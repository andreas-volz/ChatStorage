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
  UNKNOWN, HELP, VERSION, DB, NAME, TEXT, CHAT_ID, ID, PRINT_CONTEXT
};

fs::path option_db_path;
string option_backend;
string option_name;
string option_text;
string option_command;
string option_subcommand;
fs::path option_media_path;
int option_id = 0;
int option_chat_id = 0;
bool option_print_context = false;

// @formatter:off
const option::Descriptor usage[] = {
    { UNKNOWN, 0, "", "", option::Arg::None,
      "USAGE: chatstorage-cli [GLOBAL OPTIONS] [COMMAND OPTIONS] <command>\n\n"
      "GLOBAL OPTIONS:" },
    { DB, 0, "", "db", Arg::Required, "    --db <path> \t\t\tPath to database (optional: enables DB access)" },
    { HELP, 0, "h", "help", option::Arg::None, "  --help, -h\t\t\tShow this help and exit" },
    { VERSION, 0, "", "version", option::Arg::None, "  --version\t\t\tShow program version and exit" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "  inspect chat\t\t\tInspect a chat by internal database ID" },
    { ID, 0, "", "id", Arg::Numeric, "    --id <int>" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\n  inspect user\t\t\tInspect a user by internal database ID" },
    { ID, 0, "", "id", Arg::Numeric, "    --id <int>" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\n  inspect message\t\t\tInspect a message by internal database ID" },
    { ID, 0, "", "id", Arg::Numeric, "    --id <int>" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\nEXAMPLES:" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "  # Memory-only import (default)\nchatstorage-cli import chats.txt\n" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\n  # DB import\nchatstorage-cli --db chatstorage.db import chat.txt\n" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\n  # Search chats by name\nchatstorage-cli --name \"Family\" search chats" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\n  # Inspect a specific chat\nchatstorage-cli --id 1 --print-context --db chatstorage.db inspect chat" },
    { 0, 0, 0, 0, 0, 0 } };
// @formatter:on

int parse_options(int argc, const char **argv)
{
  argc -= (argc > 0);
  argv += (argc > 0); // skip program name argv[0] if present
  option::Stats stats(usage, argc, argv);
  std::unique_ptr<option::Option[]> options(new option::Option[stats.options_max]), buffer(new option::Option[stats.buffer_max]);
  option::Parser parse(usage, argc, argv, options.get(), buffer.get());

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

  if (options[PRINT_CONTEXT])
  {
    option_print_context = true;
  }

  if (options[DB].count() > 0)
  {
    option_db_path = std::filesystem::path(options[DB].arg);
  }

  if (options[NAME].count() > 0)
  {
    option_name = options[NAME].arg;
  }

  if (options[TEXT].count() > 0)
  {
    option_text = options[TEXT].arg;
  }

  if (options[CHAT_ID].count() > 0)
  {
    option_chat_id = atoi(options[CHAT_ID].arg);
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
      case 1:
        option_subcommand = parse.nonOption(i);
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

void inspectMessage(const Message &message)
{
  cout << "\nMessage Inspection:" << endl;
  cout << "--------------------" << endl;
  printKV("getRuntimeId():", message.getRuntimeId());
  printKV("getDatabaseId():", message.getDatabaseId());
  printKV("getChatRuntimeId():", message.getChatRuntimeId());
  printKV("getChatDatabaseId():", message.getChatDatabaseId());
  printKV("getSenderRuntimeId():", message.getSenderRuntimeId());
  printKV("getSenderDatabaseId():", message.getSenderDatabaseId());
  printKV("getMediaRuntimeId():", message.getMediaRuntimeId());
  printKV("getMediaDatabaseId():", message.getMediaDatabaseId());
  printKV("getTimestamp():", message.getTimestamp());
  printKV("getDatabaseId():", message.getDatabaseId());
  printKV("getType():", static_cast<int>(message.getDatabaseId()));
  printKV("getText():", message.getText());
}

void inspectUser(const User &user)
{
  cout << "\n[User Inspection]" << endl;
  printKV("getRuntimeId():", user.getRuntimeId());
  printKV("getDatabaseId():", user.getDatabaseId());
  printKV("getName():", user.getName());
}

void inspectChat(const Chat &chat)
{
  cout << "\n[Chat Inspection]" << endl;
  printKV("getRuntimeId():", chat.getRuntimeId());
  printKV("getDatabaseId():", chat.getDatabaseId());
  printKV("getName():", chat.getName());
  printKV("getSource():", static_cast<int>(chat.getDatabaseId()));
}

void printChatContext(const ChatContext &ctx)
{
  cout << "Chat Info:" << endl;
  if (ctx.getChat())
  {
    //cout << "Name: " <<  ctx.getChat()->getName() << " (id:" << ctx.getChat()->getRuntimeId() << ")" <<  endl;
    inspectChat(*ctx.getChat());
  }
  cout << endl;

  cout << "Users in the context: " << endl;
  for (auto user_it = ctx.getUserList().begin(); user_it != ctx.getUserList().end(); user_it++)
  {
    const User &user = *user_it;
    //cout << "-> " << user.getName() << " (id:" << user.getRuntimeId() << ")" << endl;
    inspectUser(user);
  }
  cout << endl;

  cout << "Messages in the context:" << endl;
  for (auto message_it = ctx.getMessageList().begin(); message_it != ctx.getMessageList().end(); message_it++)
  {
    const Message &message = *message_it;
    //const User &user = ctx.getUserById(message.getSenderRuntimeId());

    //cout << unixToLocalIso(message.getTimestamp()) << " - " << user.getName() << "(" << user.getRuntimeId() << "): " << message.getText() << endl;
    //cout << "\\->getSenderRuntimeId: " << message.getSenderRuntimeId() << endl;
    //cout << "\\->getSenderDatabaseId: " << message.getSenderDatabaseId() << endl;
    inspectMessage(message);
  }
  cout << endl;
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

  for (auto chat_entry : chat_entry_list)
  {
    chat_context = chat_storage.loadByChatEntry(chat_entry);
    printChatContext(*chat_context);
  }

  //option::printUsage(std::cout, usage);

  return 0;
}
