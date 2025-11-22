#pragma once
#include <Enigma/Core/Core.h>
#include <Enigma/Core/Types.h>
#include <Enigma/Core/System.h>
#include <vector>
#include <variant>

// I have failed to write a YAML parser, I will come back to this; but I want to move on
// so I'm using the library "yaml-cpp"

namespace Enigma::Serialization::YAML {

	// sequences are arrays

	//enum class Style {
	//	None = 0,
	//	Plain,
	//	DoubleQuoted,
	//	SingleQuoted
	//};

	//enum class BlockType {
	//	None = 0,
	//	Scalar,
	//	Map,
	//	Sequence,
	//	Comment,
	//	Document
	//};
	//enum class ScalarType {
	//	None = 0,
	//	Integer,
	//	FloatingPoint,
	//	Null,
	//	Boolean,
	//	String,
	//	Timestamp // TODO: impl
	//};

	//struct Scalar
	//{
	//	ScalarType type = ScalarType::None;

	//	std::variant<i64, f64, bool, std::string> data;

	//	Scalar() = default;
	//	Scalar(ScalarType type);
	//	Scalar(i32 value);
	//	Scalar(i64 value);
	//	Scalar(f32 value);
	//	Scalar(f64 value);
	//	Scalar(bool value);

	//	Scalar(const i8* value);
	//	Scalar(const std::string& value);

	//	bool IsNull();
	//};
	//struct Sequence {
	//	std::vector<u64> indices;
	//};
	//struct Mapping {
	//	std::map<std::string, u64> mapping; // TODO: complex keys
	//};
	//struct Comment {
	//	std::string content;
	//	bool onLine; // Is the comment on the last line
	//};

	//struct DataTreeNode {
	//	BlockType type;
	//	std::vector<DataTreeNode> children;
	//	std::variant<Scalar, Comment> block;
	//	Sequence sequence;
	//	Mapping mapping;

	//	void PushBack(const DataTreeNode& node);

	//	DataTreeNode& At(u64 id);
	//	DataTreeNode& operator[] (const std::string& name);
	//	const DataTreeNode& operator[] (const std::string& name) const;

	//	DataTreeNode() = default;
	//	DataTreeNode(BlockType type);
	//	DataTreeNode(const Scalar& value) : type(BlockType::Scalar) { block = value; }
	//	DataTreeNode(const Comment& value) : type(BlockType::Comment) { block = value; }
	//};

	//extern DataTreeNode InvalidDataTreeNode;

	//struct YAMLError {
	//	bool occurred; // If an error occurred

	//	u64 line; // The line the error occurred on in the file
	//	std::string message;

	//	YAMLError(): occurred(false) { }
	//	YAMLError(bool occurred) : occurred(occurred) { }
	//	YAMLError(u64 line, const std::string& message) : occurred(true), line(line), message(message) { }

	//	operator bool() { return occurred; }
	//};

	//struct YAMLDocument {
	//	std::string tag;
	//	std::string content;
	//	u64 startLine;
	//};

	//class YAML {
	//private:
	//	struct Consumer {
	//		std::string buffer;
	//		u64 pointer = 0;
	//		u64 startLine = 0;

	//		// Returns the char at the buffers pointer, then increments the pointer
	//		char Eat();
	//		// Returns the char at the buffers pointer
	//		char Peek();

	//		u64 GetLineNumber();
	//	};

	//public:
	//	static YAMLError LoadFile(const std::string& filename, DataTreeNode& tree);

	//private:
	//	// Returns the line number a character at index is on in text
	//	static u64 GetLineNumber(u64 index, const std::string& text);

	//	static YAMLError ParseDirectives(Consumer& consumer, YAMLDocument& document);
	//	static YAMLError ParseTag(Consumer& consumer, std::string& tag, const YAMLDocument& document);
	//	static YAMLError ParseComment(Consumer& consumer, DataTreeNode& tree, const YAMLDocument& document);
	//	static YAMLError ParseSequence(Consumer& consumer, DataTreeNode& tree, const YAMLDocument& document);
	//	static YAMLError ParseDocument(const YAMLDocument& document, DataTreeNode& tree);

	//private:
	//	inline static const std::string_view c_WhiteSpace = "\n\t\0\r ";
	//};
}