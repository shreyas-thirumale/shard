#include <gtest/gtest.h>
#include "../src/lexer/Lexer.h"
#include "../src/parser/Parser.h"
#include "../src/ast/CommandNodes.h"

// Helper: lex + parse
static ASTNodePtr parse(const std::string& input) {
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    return parser.parse();
}

// ── Navigation ────────────────────────────────────────────────────────────────

TEST(ParserTest, ChangeDirectoryVariant1) {
    auto node = parse("change directory Documents");
    auto* cd = dynamic_cast<ChangeDirectoryNode*>(node.get());
    ASSERT_NE(cd, nullptr);
    EXPECT_EQ(cd->destination, "Documents");
}

TEST(ParserTest, ChangeDirectoryVariant2) {
    auto node = parse("go to Downloads");
    auto* cd = dynamic_cast<ChangeDirectoryNode*>(node.get());
    ASSERT_NE(cd, nullptr);
    EXPECT_EQ(cd->destination, "Downloads");
}

TEST(ParserTest, ChangeDirectoryBack) {
    auto node = parse("go back");
    auto* cd = dynamic_cast<ChangeDirectoryNode*>(node.get());
    ASSERT_NE(cd, nullptr);
    EXPECT_EQ(cd->destination, "..");
}

TEST(ParserTest, ListFiles) {
    auto node = parse("list files");
    EXPECT_NE(dynamic_cast<ListDirectoryNode*>(node.get()), nullptr);
}

TEST(ParserTest, ShowCurrentDirectory) {
    auto node = parse("show current directory");
    EXPECT_NE(dynamic_cast<CurrentDirectoryNode*>(node.get()), nullptr);
}

// ── File Operations ───────────────────────────────────────────────────────────

TEST(ParserTest, CopyFile) {
    auto node = parse("copy report.pdf to Backup");
    auto* cp = dynamic_cast<CopyFileNode*>(node.get());
    ASSERT_NE(cp, nullptr);
    EXPECT_EQ(cp->source, "report.pdf");
    EXPECT_EQ(cp->destination, "Backup");
}

TEST(ParserTest, MoveFile) {
    auto node = parse("move image.png into Pictures");
    auto* mv = dynamic_cast<MoveFileNode*>(node.get());
    ASSERT_NE(mv, nullptr);
    EXPECT_EQ(mv->source, "image.png");
    EXPECT_EQ(mv->destination, "Pictures");
}

TEST(ParserTest, DeleteFile) {
    auto node = parse("delete old.txt");
    auto* del = dynamic_cast<DeleteFileNode*>(node.get());
    ASSERT_NE(del, nullptr);
    EXPECT_EQ(del->target, "old.txt");
}

TEST(ParserTest, RenameFile) {
    auto node = parse("rename draft.txt to final.txt");
    auto* ren = dynamic_cast<RenameFileNode*>(node.get());
    ASSERT_NE(ren, nullptr);
    EXPECT_EQ(ren->source, "draft.txt");
    EXPECT_EQ(ren->newName, "final.txt");
}

TEST(ParserTest, CreateFile) {
    auto node = parse("create file notes.txt");
    auto* cr = dynamic_cast<CreateFileNode*>(node.get());
    ASSERT_NE(cr, nullptr);
    EXPECT_EQ(cr->name, "notes.txt");
}

// ── Directory Operations ──────────────────────────────────────────────────────

TEST(ParserTest, CreateFolder) {
    auto node = parse("create folder Projects");
    auto* cr = dynamic_cast<CreateDirectoryNode*>(node.get());
    ASSERT_NE(cr, nullptr);
    EXPECT_EQ(cr->name, "Projects");
}

TEST(ParserTest, DeleteFolder) {
    auto node = parse("delete folder Temp");
    auto* del = dynamic_cast<DeleteDirectoryNode*>(node.get());
    ASSERT_NE(del, nullptr);
    EXPECT_EQ(del->target, "Temp");
}

// ── Meta ──────────────────────────────────────────────────────────────────────

TEST(ParserTest, HelpNoTopic) {
    auto node = parse("help");
    auto* h = dynamic_cast<HelpNode*>(node.get());
    ASSERT_NE(h, nullptr);
    EXPECT_TRUE(h->topic.empty());
}

TEST(ParserTest, HelpWithTopic) {
    auto node = parse("help copy");
    auto* h = dynamic_cast<HelpNode*>(node.get());
    ASSERT_NE(h, nullptr);
    EXPECT_EQ(h->topic, "copy");
}

TEST(ParserTest, Exit) {
    auto node = parse("exit");
    EXPECT_NE(dynamic_cast<ExitNode*>(node.get()), nullptr);
}

TEST(ParserTest, UnknownCommandThrows) {
    EXPECT_THROW(parse("frobnicate something"), ParseError);
}
