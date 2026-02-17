// project public API
#include "chatstorage/ChatStorage.h"
#include "chatstorage/Chat.h"

// project internal
#include "common/Logger.h"
#include "common/optparser.h"
#include "common/FileUtil.h"
#include "common/StringUtil.h"

// system
#include <iostream>
#include <fstream>
#include <memory>
#include <map>

using namespace std;
using namespace StringUtil;

static Logger logger("ChatStorage.chatstorage-inspect");

enum optionIndex
{
  UNKNOWN, HELP, VERSION, DB, NAME, ID, LIST_CHATS
};

fs::path option_db_path;
string option_backend;
string option_name;
string option_text;
string option_command;
fs::path option_media_path;
bool option_list_chats = false;
int option_id = -1;

// @formatter:off
const option::Descriptor usage[] = {
    { UNKNOWN, 0, "", "", option::Arg::None,
      "USAGE: chatstorage-view [OPTIONS] \n\n"
      "OPTIONS:" },
    { DB, 0, "", "db", Arg::Required, "  --db <path> \t\t\tPath to database" },
    { HELP, 0, "h", "help", option::Arg::None, "  --help, -h\t\t\tShow this help and exit" },
    { VERSION, 0, "", "version", option::Arg::None, "  --version\t\t\tShow program version and exit" },
    { ID, 0, "", "id", Arg::Numeric, "  --id <int> \t\t\tView chat with this database id" },
    { NAME, 0, "", "name", Arg::Required, "  --name <string> \t\t\tView chat with that name" },
    { LIST_CHATS, 0, "", "list-chats", option::Arg::None, "  --list-chats\t\t\tList all available chats" },
    { UNKNOWN, 0, "", "", option::Arg::None,
          "\nEXAMPLES:" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\n  # View a specific chat by id\nchatstorage-view --db chatstorage.db --id 1" },
    { UNKNOWN, 0, "", "", option::Arg::None,
          "\n  # View a specific chat by name\nchatstorage-view --db chatstorage.db --name 'My Special Chat'" },
    { 0, 0, 0, 0, 0, 0 } };
// @formatter:on

int parse_options(int argc, const char **argv)
{
  argc -= (argc > 0);
  argv += (argc > 0); // skip program name argv[0] if present
  option::Stats stats(usage, argc, argv);
  std::unique_ptr<option::Option[]> options(new option::Option[stats.options_max]), buffer(new option::Option[stats.buffer_max]);
  option::Parser parse(usage, argc, argv, options.get(), buffer.get());

  if (argc == 0)
  {
    option::printUsage(std::cout, usage);
    exit(0);
  }

  if (parse.error())
    exit(0);

  if (options[HELP])
  {
    option::printUsage(std::cout, usage);
    exit(0);
  }

  if (options[VERSION])
  {
    cout << "TODO: display version" << endl;
    exit(0);
  }

  if (options[LIST_CHATS])
  {
    option_list_chats = true;
  }

  if (options[DB].count() > 0)
  {
    option_db_path = std::filesystem::path(options[DB].arg);
  }

  if (options[NAME].count() > 0)
  {
    option_name = options[NAME].arg;
  }

  if (options[ID].count() > 0)
  {
    option_id = atoi(options[ID].arg);
  }

  // parse options
  for (option::Option *opt = options[UNKNOWN]; opt; opt = opt->next())
    std::cout << "Unknown option: " << opt->name << "\n";

  // parse commands
  for (int i = 0; i < parse.nonOptionsCount(); ++i)
  {
    switch (i)
    {
      case 0:
        option_command = parse.nonOption(i);
        break;
      default:
        break;
    }
  }

  return 0;
}

std::string unixToLocalIso(int64_t unix_seconds)
{
  std::time_t t = static_cast<std::time_t>(unix_seconds);
  std::tm tm {};

#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

void printChatContext(const ChatContext &ctx)
{
  for (const auto &message : ctx.getMessageList())
  {

    const User &user = ctx.getUserBySenderRuntimeId(message.getSenderRuntimeId());

    cout << unixToLocalIso(message.getTimestamp()) << " - " << user.getName() << ": " << message.getText() << endl;
  }
}

int main(int argc, const char **argv)
{
#ifdef HAVE_LOG4CXX
  const std::string LOG4CXX_PROPERTIES = "logging.prop";
  if (fs::exists(LOG4CXX_PROPERTIES))
  {
    log4cxx::PropertyConfigurator::configure(LOG4CXX_PROPERTIES);
  }
  else
  {
    logger.off();
  }
#endif // HAVE_LOG4CXX

  parse_options(argc, argv);

  ChatStorage chat_storage(option_db_path, option_media_path);

  vector<ChatEntry> chat_entry_list = chat_storage.getChatEntryList();

  unique_ptr<ChatContext> chat_context;

  if (option_id > 0)
  {
    for (const auto &chat_entry : chat_entry_list)
    {
      if (chat_entry.database_id == option_id)
      {
        chat_context = chat_storage.loadByChatEntry(chat_entry);
      }
    }
  }
  else if (!option_name.empty())
  {
    for (const auto &chat_entry : chat_entry_list)
    {
      if (chat_entry.name == option_name)
      {
        chat_context = chat_storage.loadByChatEntry(chat_entry);
      }
    }
  }


  if (chat_context)
  {
    printChatContext(*chat_context);
  }

  if (option_list_chats)
  {
    cout << "Chat List: " << endl;
    for (const auto &chat_entry : chat_entry_list)
    {
      cout << to_string(chat_entry.database_id) << ": " << chat_entry.name << endl;
    }
  }

  return 0;
}
