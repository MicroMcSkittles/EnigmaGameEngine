//#include "EnigmaSerialization/YAML.h"
//#include <string.h>
//#include <fstream>
//#include <sstream>
//
//namespace Enigma::Serialization::YAML {
//
//	// Scalar constructors
//	Scalar::Scalar(ScalarType type): type(type) { }
//	Scalar::Scalar(i32 value) {
//		type = ScalarType::Integer;
//		data = static_cast<i64>(value);
//	}
//	Scalar::Scalar(i64 value)
//	{
//		type = ScalarType::Integer;
//		data = value;
//	}
//	Scalar::Scalar(f32 value) {
//		type = ScalarType::FloatingPoint;
//		data = static_cast<f64>(value);
//	}
//	Scalar::Scalar(f64 value)
//	{
//		type = ScalarType::FloatingPoint;
//		data = value;
//	}
//	Scalar::Scalar(bool value)
//	{
//		type = ScalarType::Boolean;
//		data = value;
//	}
//	Scalar::Scalar(const i8* value)
//	{
//		type = ScalarType::String;
//		data = std::string(value);
//	}
//	Scalar::Scalar(const std::string& value)
//	{
//		type = ScalarType::String;
//		data = value;
//	}
//	bool Scalar::IsNull()
//	{
//		return type == ScalarType::Null;
//	}
//
//	DataTreeNode InvalidDataTreeNode = DataTreeNode(BlockType::None);
//
//	void DataTreeNode::PushBack(const DataTreeNode& node)
//	{
//		if (type != BlockType::Sequence) return;
//
//		sequence.indices.push_back(children.size());
//		children.push_back(node);
//	}
//
//	DataTreeNode& DataTreeNode::At(u64 id) {
//		if (type != BlockType::Sequence) return InvalidDataTreeNode;
//		if (id >= sequence.indices.size()) return InvalidDataTreeNode;
//		return children[sequence.indices[id]];
//	}
//	DataTreeNode& DataTreeNode::operator[] (const std::string& name) {
//		if (type != BlockType::Map) return InvalidDataTreeNode;
//		if (!mapping.mapping.count(name)) {
//			mapping.mapping.insert({ name, children.size() });
//			children.push_back(DataTreeNode());
//		}
//		return children[mapping.mapping[name]];
//	}
//	const DataTreeNode& DataTreeNode::operator[] (const std::string& name) const {
//		if (type != BlockType::Map) return InvalidDataTreeNode;
//		if (!mapping.mapping.count(name)) return InvalidDataTreeNode;
//		return children[mapping.mapping.at(name)];
//	}
//
//	DataTreeNode::DataTreeNode(BlockType type): type(type)
//	{
//		switch (type)
//		{
//		case Enigma::Serialization::YAML::BlockType::Scalar:   block = Scalar();   break;
//		case Enigma::Serialization::YAML::BlockType::Comment:  block = Comment();  break;
//		}
//	}
//
//	// Helper functions
//	u64 YAML::GetLineNumber(u64 index, const std::string& text)
//	{
//		// Count the number of end lines before the index in text
//		u64 lineNumber = 1, currentIndex = 0;
//		while ((currentIndex = text.find_first_of("\n", currentIndex)) <= index) {
//			lineNumber += 1;
//			currentIndex += 1;
//		}
//		return lineNumber;
//	}
//	
//	char YAML::Consumer::Eat()
//	{
//		return buffer[pointer++];
//	}
//	char YAML::Consumer::Peek()
//	{
//		return buffer[pointer];
//	}
//
//	u64 YAML::Consumer::GetLineNumber()
//	{
//		return startLine + YAML::GetLineNumber(pointer, buffer);
//	}
//
//	// Loading functions
//	YAMLError YAML::LoadFile(const std::string& filename, DataTreeNode& tree)
//	{
//		// Open YAML file
//		std::ifstream file;
//		file.open(filename);
//		if (!file.is_open()) return { 0, "Failed to open file" };
//		
//		// Copy file content to a string
//		std::stringstream fileBuffer;
//		fileBuffer << file.rdbuf();
//		std::string fileContent = fileBuffer.str();
//
//		// Close the file
//		file.close();
//
//		YAMLDocument document;
//		Consumer consumer = { fileContent };
//
//		// Parse the directives if they exist
//		consumer.pointer = consumer.buffer.find_first_of("%");
//		if (consumer.pointer != std::string::npos) {
//			ParseDirectives(consumer, document);
//
//			// Make sure there is a document separator
//			u64 documentSeparator = consumer.buffer.find("---", consumer.pointer);
//			consumer.pointer = consumer.buffer.find_first_not_of(c_WhiteSpace, consumer.pointer + 1);
//			if (consumer.pointer != documentSeparator) {
//				return { consumer.GetLineNumber(), "Expected document separator after directives" };
//			}
//
//			// Skip to the document separator
//			consumer.pointer = documentSeparator;
//		}
//
//		// Skip document separator if it exists
//		consumer.pointer = consumer.buffer.find("---", consumer.pointer);
//		if (consumer.pointer != std::string::npos) {
//			// Parse the document tag if it exists
//			consumer.pointer += 3;
//			YAMLError error = ParseTag(consumer, document.tag, document);
//			if (error) return error;
//		}
//		u64 startLocation = consumer.pointer;
//
//		// Find the end of the first document
//		consumer.pointer = std::min(
//			consumer.buffer.find("---", startLocation),
//			consumer.buffer.find("...", startLocation)
//		);
//		if (consumer.pointer == std::string::npos) consumer.pointer = fileContent.size();
//
//		// Create the document
//		document.startLine = GetLineNumber(startLocation, fileContent);
//		document.content = std::string(
//			consumer.buffer.begin() + startLocation, 
//			consumer.buffer.begin() + consumer.pointer
//		);
//
//		// Parse document
//		DataTreeNode documentNode;
//		YAMLError error = ParseDocument(document, documentNode);
//
//		tree = documentNode;
//
//		return error;
//	}
//	
//	YAMLError YAML::ParseDirectives(Consumer& consumer, YAMLDocument& document)
//	{
//		u64 lastValidLocation = 0;
//		while ((consumer.pointer = consumer.buffer.find_first_of("%", consumer.pointer)) != std::string::npos) {
//			// Get directive tag
//			u64 startLocation = consumer.pointer;
//			consumer.pointer = consumer.buffer.find_first_of(" \n", startLocation);
//			
//			if (consumer.Peek() == '\n') return { consumer.GetLineNumber(), "Directive defined but not declared" };
//			
//			std::string tag = std::string(
//				consumer.buffer.begin() + startLocation + 1,
//				consumer.buffer.begin() + consumer.pointer
//			);
//
//			// Get directive definition
//			startLocation = consumer.buffer.find_first_not_of(c_WhiteSpace, consumer.pointer);
//			consumer.pointer = consumer.buffer.find_first_of("\n", startLocation);
//			std::string directive = std::string(
//				consumer.buffer.begin() + startLocation + 1,
//				consumer.buffer.begin() + consumer.pointer
//			);
//
//			lastValidLocation = consumer.pointer;
//		}
//		consumer.pointer = lastValidLocation;
//		return false;
//	}
//	YAMLError YAML::ParseTag(Consumer& consumer, std::string& tag, const YAMLDocument& document)
//	{
//		// Check if there is a tag
//		u64 startLocation = consumer.buffer.find_first_not_of(c_WhiteSpace, consumer.pointer) + 1;
//		if (consumer.buffer[startLocation] != '!') return false;
//
//		// TODO: impl the tag shorthand stuff
//		// Get the tag
//		consumer.pointer = consumer.buffer.find_first_of(" \n", startLocation + 1);
//		tag = std::string(
//			consumer.buffer.begin() + startLocation,
//			consumer.buffer.begin() + consumer.pointer
//		);
//
//		return false;
//	}
//
//	YAMLError YAML::ParseComment(Consumer& consumer, DataTreeNode& tree, const YAMLDocument& document)
//	{
//		u64 startLocation = consumer.pointer + 1;
//		consumer.pointer = consumer.buffer.find_first_of("\n", startLocation);
//
//		Comment comment;
//		comment.content = std::string(
//			consumer.buffer.begin() + startLocation,
//			consumer.buffer.begin() + consumer.pointer
//		);
//		comment.onLine = false;
//
//		DataTreeNode node = comment;
//		tree.children.push_back(node);
//
//		return false;
//	}
//	YAMLError YAML::ParseSequence(Consumer& consumer, DataTreeNode& tree, const YAMLDocument& document)
//	{
//		u64 indent = consumer.buffer.find_last_of("\n");
//		if (indent == std::string::npos) indent = consumer.pointer;
//
//		std::string tag;
//		ParseTag(consumer, tag, document);
//
//
//
//		return false;
//	}
//
//	// Parsing functions
//	YAMLError YAML::ParseDocument(const YAMLDocument& document, DataTreeNode& tree)
//	{
//		tree = BlockType::Document;
//
//		// Create consumer
//		Consumer consumer;
//		consumer.buffer = document.content;
//		consumer.startLine = document.startLine;
//
//		for (; consumer.pointer < consumer.buffer.size(); consumer.pointer++) {
//			// Find start and end of the line
//			u64 startLocation = consumer.pointer;
//			consumer.pointer = std::min(consumer.buffer.find_first_of("\n", consumer.pointer), consumer.buffer.size());
//
//			// Get the specifier character
//			consumer.pointer = consumer.buffer.find_first_not_of(c_WhiteSpace, consumer.pointer);
//			if (consumer.pointer == std::string::npos) break;
//			char specifier = consumer.Peek();
//
//			if (specifier == '#') ParseComment(consumer, tree, document);
//			else if (specifier == '-') ParseSequence(consumer, tree, document);
//			else if (isalpha(specifier)) {
//
//			}
//		}
//
//		return false;
//	}
//	
//}