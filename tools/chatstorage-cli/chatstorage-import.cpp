// project public API
#include "chatstorage/ChatStorage.h"
#include "chatstorage/ChatStorageImporter.h"
#include "chatstorage/Chat.h"

// project internal
#include "common/pacman.h"
#include "common/optparser.h"
#include "common/Logger.h"
#include "common/FileUtil.h"
#include "common/StringUtil.h"


// system
#include <iostream>
#include <fstream>
#include <memory>
#include <map>

using namespace std;
using namespace StringUtil;

static Logger logger("ChatStorage.chatstorage-import");

enum optionIndex
{
  UNKNOWN, HELP, VERSION, DB, BACKEND, LIST_BACKENDS, NAME, TEXT, CHAT_ID, ID, PRINT_CONTEXT, MEDIA_PATH, MAP_USER, USER_DEFAULT, INPUT_FILE
};

fs::path option_db_path;
string option_backend;
string option_name = "<no name>";
string option_text;
fs::path option_input_file;
string option_command;
string option_subcommand;
fs::path option_media_path;
int option_id = 0;
int option_chat_id = 0;
bool option_print_context = false;
vector<pair<string, int>> option_user_mapping;
bool option_user_default_new = true;

// @formatter:off
const option::Descriptor usage[] = {
    { UNKNOWN, 0, "", "", option::Arg::None,
      "USAGE: chatstorage-cli [GLOBAL OPTIONS] [COMMAND OPTIONS] <command>\n\n"
      "GLOBAL OPTIONS:" },
    { DB, 0, "", "db", Arg::Required, "    --db <path> \t\t\tPath to database (optional: enables DB access)" },
    { MEDIA_PATH, 0, "", "media", Arg::Required, "    --media <path> \t\t\tPath to media persistence folder (optional: enables media import/read)" },
    { HELP, 0, "h", "help", option::Arg::None, "  --help, -h\t\t\tShow this help and exit" },
    { VERSION, 0, "", "version", option::Arg::None, "  --version\t\t\tShow program version and exit" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "  import <path>\t\t\tImport chat export from file or directory (default: memory-only; no database writes unless --db is set)" },
    { BACKEND, 0, "", "backend", Arg::Required, "    --backend <name> \t\t\tImport backend / format\n\t\t\t(default: auto)" },
    { NAME, 0, "", "name", Arg::Required, "    --name <string> \t\t\tImport chat with that name" },
    { LIST_BACKENDS, 0, "", "list-backends", option::Arg::None, "    --list-backends\t\t\tList all available import backends and exit" },
    { PRINT_CONTEXT, 0, "", "print-context", option::Arg::None, "    --print-context\t\t\tPrint the complete loaded ChatContext" },
    { MAP_USER, 0, "", "map-user", Arg::Required, "    --map-user\t\t\tMap imported user to existing user ID ('name:1' -> could be used multiple times)" },
    { INPUT_FILE, 0, "", "input-file", Arg::Required, "    --input-file\t\t\tInput file for import parser" },
    { USER_DEFAULT, 0, "", "user-default", Arg::Required, "Default strategy for unmapped users (possible: auto/new; default: new)"},
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\nEXAMPLES:" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "  # Memory-only import (default)\nchatstorage-import --input-file chats.txt\n" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\n  # DB import\nchatstorage-import --db chatstorage.db --input-file chat.txt\n" },
    { UNKNOWN, 0, "", "", option::Arg::None,
      "\n  # Search chats by name\nchatstorage-import --name 'Family' --db chatstorage.db --input-file chat.txt\n" },
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

  if (options[MEDIA_PATH].count() > 0)
  {
    option_media_path = std::filesystem::path(options[MEDIA_PATH].arg);
  }

  if (options[BACKEND].count() > 0)
  {
    cerr << "TODO: only one backend supported at the moment" << endl;
    option_backend = options[BACKEND].arg;
  }

  if (options[USER_DEFAULT].count() > 0)
  {
    if (string(options[USER_DEFAULT].arg) == "new")
    {
      option_user_default_new = true;
    }
    else if (string(options[USER_DEFAULT].arg) == "auto")
    {
      option_user_default_new = false;
    }
  }

  if (options[LIST_BACKENDS])
  {
    cerr << "TODO: only one backend supported at the moment" << endl;
    exit(0);
  }

  if (options[NAME].count() > 0)
  {
    option_name = options[NAME].arg;
  }

  if (options[TEXT].count() > 0)
  {
    option_text = options[TEXT].arg;
  }

  if (options[INPUT_FILE].count() > 0)
  {
    option_input_file = options[INPUT_FILE].arg;
  }

  if (options[CHAT_ID].count() > 0)
  {
    option_chat_id = atoi(options[CHAT_ID].arg);
  }

  if (options[ID].count() > 0)
  {
    option_id = atoi(options[ID].arg);
  }

  for (option::Option *opt = options[MAP_USER]; opt; opt = opt->next())
  {
    std::string arg(opt->arg);
    auto pos = arg.find(':');
    if (pos != std::string::npos)
    {
      std::string name = arg.substr(0, pos);
      int id = std::stoi(arg.substr(pos + 1));
      option_user_mapping.emplace_back(name, id);
    }
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

  ChatContext import_chat_context;
  ChatStorageImporter::importFromFile(option_input_file, ImportConfig {option_name, ChatSource::FormatA, option_user_mapping }, import_chat_context);

  chat_storage.save(import_chat_context, option_input_file.parent_path());

  if (option_print_context)
  {
    printChatContext(import_chat_context);
  }

  //option::printUsage(std::cout, usage);

  return 0;
}
