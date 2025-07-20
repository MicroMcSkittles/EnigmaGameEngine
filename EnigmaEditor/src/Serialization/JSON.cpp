#include "Serialization/JSON.h"

#include <Enigma/Core/Core.h>
#include <Enigma/Core/System.h>

#include <iomanip>

namespace Enigma {
	namespace Editor {
		namespace JSON {

			// Data Tree Value
			bool DataTreeValue::IsNull()
			{
				return type == DataTreeType::Null;
			}
			DataTreeValue::operator int()
			{
				if (type != DataTreeType::Int) {
					LOG_WARNING("Data Tree Value is not an int");
					return 0;
				}
				return std::stoi(value);
			}
			DataTreeValue::operator float()
			{
				if (type != DataTreeType::Float) {
					LOG_WARNING("Data Tree Value is not a float");
					return 0.0f;
				}
				return std::stof(value);
			}
			DataTreeValue::operator bool()
			{
				if (type != DataTreeType::Boolean) {
					LOG_WARNING("Data Tree Value is not a boolean");
					return false;
				}
				return value == "true";
			}
			DataTreeValue::operator std::string()
			{
				if (type != DataTreeType::String) {
					LOG_WARNING("Data Tree Value is not a string");
					return "";
				}
				return value;
			}
		
			// Data Tree Node
			DataTreeNode::DataTreeNode(int v)
			{
				value.type = DataTreeType::Int;
				value.value = std::to_string(v);
			}
			DataTreeNode::DataTreeNode(float v)
			{
				value.type = DataTreeType::Float;
				value.value = std::to_string(v);
			}
			DataTreeNode::DataTreeNode(bool v)
			{
				value.type = DataTreeType::Boolean;
				value.value = ((v) ? "true" : "false");
			}
			DataTreeNode::DataTreeNode(const std::string& v)
			{
				value.type = DataTreeType::String;
				value.value = v;
			}
			DataTreeNode::DataTreeNode(const char* v) {
				value.type = DataTreeType::String;
				value.value = v;
			}

			DataTreeNode& DataTreeNode::At(int id)
			{
				if (id < 0 || id > elements.size()) {
					LOG_WARNING("Data tree node element ID out of bounds %i", id);
					DataTreeNode nullNode;
					return nullNode;
				}
				return elements.at(id);
			}

			DataTreeNode& DataTreeNode::operator[](const std::string& name)
			{
				if (!children.count(name)) {
					children.insert({ name, DataTreeNode() });
				}
				return children.at(name);
			}
			const DataTreeNode& DataTreeNode::operator[](const std::string& name) const
			{
				if (!children.count(name)) {
					LOG_WARNING("Data tree node does not contain child node ( %s )", name.c_str());
					DataTreeNode nullNode;
					return nullNode;
				}
				return children.at(name);
			}

			// JSON Consumer
			char JSON::Consumer::eat() {
				char c;
				buffer >> c;
				return c;
			}
			char JSON::Consumer::peek()
			{
				return buffer.peek();
			}
			void JSON::Consumer::push(char c)
			{
				buffer.seekp(-1, std::ios::cur);
				buffer << c;
			}

			// JSON Serialization
			std::string JSON::Tab(int tabDepth) {
				std::stringstream tab;
				int depth = tabDepth + 1;
				// Use the iomanip library to create a string with tabDepth number of tabs
				tab << std::setw(depth) << std::setfill('\t') << std::right << "";
				return tab.str();
			}
			bool JSON::Serialize(const std::string& filename, DataTreeNode* tree) {
				// Make sure the first node is a object, JSON requires the first node to be a obejct
				if (tree->value.type != DataTreeType::Object) {
					LOG_SOFT_ERROR("Failed to serialize JSON data tree node to file ( %s ), Invailed data tree node", filename.c_str());
					return false;
				}

				// Open file
				std::ofstream file;
				file.open(filename);
				if (!file.is_open()) {
					LOG_SOFT_ERROR("Failed to serialize JSON data tree node to file ( %s ), failed to open file", filename.c_str());
					return false;
				}

				// Save JSON data tree to file
				SerializeObject(file, tree, 0);

				// Close file
				file.close();
			}
			void JSON::SerializeObject(std::ofstream& file, DataTreeNode* tree, int tabDepth) {
				
				if (tree->children.empty()) {
					file << "{ }";
					return;
				}
				
				// Push opening bracket
				file << "{";
				if (!(tree->format.flags & Collapse)) file << '\n';

				for (int i = 0; i < tree->format.childOrder.size(); ++i) {
					std::string& name = tree->format.childOrder[i];
					DataTreeNode& child = tree->children[name];
					// Push node name
					if (child.format.flags & Indent)file << Tab(tabDepth);
					SerializeString(file, name);
					file << ": ";

					if (tree->format.flags & CarryToChildren) child.format.flags = tree->format.flags;
					if (tree->format.flags & Collapse) child.format.flags &= ((uint8_t)(-1)) - ((uint8_t)Indent);

					// Push node data
					if (child.value.type == DataTreeType::String) SerializeString(file, child.value.value); //file << "\"" << child.value.value << "\"";
					else if (child.value.type == DataTreeType::Null) file << "null";
					else if (child.value.type == DataTreeType::Array) SerializeArray(file, &child, tabDepth + 1);
					else if (child.value.type == DataTreeType::Object) SerializeObject(file, &child, tabDepth + 1);
					else file << child.value.value;

					// Push comma if there are more nodes
					if (i < tree->format.childOrder.size() - 1) {
						file << ",";
						if (!(tree->format.flags & Collapse)) file << '\n';
					}
				}
				// Push closing bracket
				if (tree->format.flags & NewLine) file << '\n';
				if (tree->format.flags & Indent)file << Tab(tabDepth - 1);
				file << "}";
			}
			void JSON::SerializeArray(std::ofstream& file, DataTreeNode* tree, int tabDepth) {
				if (tree->elements.empty()) {
					file << "[ ]";
					return;
				}
				
				// Push opening bracket
				file << "[";
				if (!(tree->format.flags & Collapse)) file << '\n';
				for (int i = 0; i < tree->elements.size(); ++i) {
					DataTreeNode& element = tree->elements[i];

					if (tree->format.flags & CarryToChildren) element.format.flags = tree->format.flags;
					if (tree->format.flags & Collapse)
						element.format.flags &= ((uint8_t)(-1)) - ((uint8_t)Indent);

					// Push tab
					if (element.format.flags & Indent) file << Tab(tabDepth);

					// Push node data
					if (element.value.type == DataTreeType::String) SerializeString(file, element.value.value); //file << "\"" << element.value.value << "\"";
					else if (element.value.type == DataTreeType::Null) file << "null";
					else if (element.value.type == DataTreeType::Array) SerializeArray(file, &element, tabDepth + 1);
					else if (element.value.type == DataTreeType::Object) SerializeObject(file, &element, tabDepth + 1);
					else file << element.value.value;
					
					// Push comma if there are more elements
					if (i != tree->elements.size() - 1) {
						file << ", ";
						if (!(tree->format.flags & Collapse)) file << '\n';
					}
				}
				// Push closing bracket
				if (!(tree->format.flags & Collapse)) file << '\n';
				if (tree->format.flags & Indent && !(tree->format.flags & Collapse)) file << Tab(tabDepth - 1);
				file << "]";
			}
			void JSON::SerializeString(std::ofstream& file, const std::string& str)
			{
				std::string rslt = str;
				size_t offset = 0;
				while ((offset = rslt.find_first_of('\\', offset)) != std::string::npos) {
					rslt.insert(rslt.begin() + offset + 1, '\\');
					offset += 2;
				}
				file << "\"" << rslt << "\"";
			}

			// JSON parsing
			bool JSON::RemoveWhiteSpace(Consumer& consumer) {
				bool quoteFlag = false; // Used to keep the white spaces inside quotes
				char lastChar = ' ';
				for (char& c : consumer.source) {
					// Flip quoteFlag if c is a " char
					if (c == '\"' && lastChar != '\\') quoteFlag = !quoteFlag;

					if (quoteFlag) {
						if (c == '\\' && lastChar != '\\') {
							lastChar = c;
							continue;
						}
						if (lastChar == '\\') {
							consumer.buffer << c;
							lastChar = ' ';
							continue;
						}
					}

					// Skip char if c is a white space and isn't in quotes
					if (isspace(c) && !quoteFlag) continue;
					// Push back c
					consumer.buffer << c;
					lastChar = c;
				}
				// if quoteFlag is true, then there must be a missing " char
				if (quoteFlag) {
					LOG_SOFT_ERROR("Failed to parse JSON source, expected closing ( \" )");
					return false;
				}

				// Tell the consumer not to skip white spaces;
				consumer.buffer >> std::noskipws;

				return true;
			}

			bool JSON::LoadFile(const std::string& filename, DataTreeNode* tree)
			{
				// Load source from file
				std::string source;
				source = Core::System::ReadFileStr(filename);
				// Parse source
				return ParseSource(source, tree);
			}
			bool JSON::ParseSource(const std::string& source, DataTreeNode* tree)
			{
				// Create comsumer
				Consumer consumer;
				consumer.source = source;
				RemoveWhiteSpace(consumer);

				// Parse source
				bool error = false;
				if (!IsObjectValid(consumer, tree, error)) {
					LOG_SOFT_ERROR("Failed to parse JSON tree node, Invalid Object");
					return false;
				}
				return !error;
			}

			bool JSON::IsObjectValid(Consumer& consumer, DataTreeNode* tree, bool& error)
			{
				// if peek returns { then this is a object
				if (consumer.peek() != '{') {
					return false;
				}
				consumer.eat();

				tree->value.type = DataTreeType::Object;

				std::string name;
				while (IsStringValid(name, consumer, error)) {
					if (consumer.peek() != ':') {
						LOG_SOFT_ERROR("Failed to parse JSON tree node, Expected token ( : ) after name  ( %s )", name.c_str());
						error = true;
						return false;
					}
					consumer.eat();

					DataTreeNode child;
					if (IsStringValid(child.value.value, consumer, error)) child.value.type = DataTreeType::String;
					else if (IsNumberValid(child.value.value, child.value.type, consumer, error));
					else if (IsBooleanValid(child.value.value, consumer, error)) child.value.type = DataTreeType::Boolean;
					else if (IsNullValid(child.value.value, consumer, error)) child.value.type = DataTreeType::Null;
					else if (IsObjectValid(consumer, &child, error));
					else if (IsArrayValid(consumer, &child, error));

					if (error) return false;

					tree->children.insert({ name, child });
					name = "";
					if (consumer.peek() == ',') {
						consumer.eat();
						continue;
					}
					if (consumer.peek() == '}') break;
					break;
				}
				if (error) return false;
				if (consumer.peek() != '}') {
					LOG_SOFT_ERROR("Failed to parse JSON tree node, Expected token ( } )");
					error = true;
					return false;
				}
				consumer.eat();
				return true;
			}
			bool JSON::IsArrayValid(Consumer& consumer, DataTreeNode* tree, bool& error)
			{
				// if peek returns [ then this is a array
				if (consumer.peek() != '[') return false;
				consumer.eat();

				tree->value.type = DataTreeType::Array;

				while (true) {

					DataTreeNode child;
					if (IsStringValid(child.value.value, consumer, error)) child.value.type = DataTreeType::String;
					else if (IsNumberValid(child.value.value, child.value.type, consumer, error));
					else if (IsBooleanValid(child.value.value, consumer, error)) child.value.type = DataTreeType::Boolean;
					else if (IsNullValid(child.value.value, consumer, error)) child.value.type = DataTreeType::Null;
					else if (IsObjectValid(consumer, &child, error));
					else if (IsArrayValid(consumer, &child, error));
					else if (consumer.peek() == ']') break;
					if (error) return false;

					tree->elements.push_back(child);
					if (consumer.peek() == ',') {
						consumer.eat();
						continue;
					}
					if (consumer.peek() == ']') break;
					break;
				}
				if (consumer.eat() != ']') {
					LOG_SOFT_ERROR("Failed to parse JSON tree node, Expected token ( ] )");
					error = true;
					return false;
				}
				return true;
			}
			bool JSON::IsStringValid(std::string& value, Consumer& consumer, bool& error)
			{
				// if peek returns " then this is a string
				if (consumer.peek() != '\"') return false;
				consumer.eat();

				char c;
				while (c = consumer.eat()) {
					if (c == '\"') break;
					value.push_back(c);
				}
				if (c != '\"') {
					LOG_SOFT_ERROR("Failed to parse JSON tree node, Expected token ( \" )");
					error = true;
					return false;
				}
				return true;
			}
			bool JSON::IsNumberValid(std::string& value, DataTreeType& type, Consumer& consumer, bool& error)
			{
				if (!isdigit(consumer.peek()) && consumer.peek() != '-') return false;
				
				type = DataTreeType::Int;

				if (consumer.peek() == '-') {
					value.push_back(consumer.peek());
					consumer.eat();
				}

				char c;
				while (c = consumer.peek()) {
					if (isdigit(c) || c == '.') {
						value.push_back(c);
						if (c == '.') type = DataTreeType::Float;
						consumer.eat();
						continue;
					}
					break;
				}
				return true;
			}
			bool JSON::IsBooleanValid(std::string& value, Consumer& consumer, bool& error)
			{
				if (consumer.peek() != 't' && consumer.peek() != 'f') return false;
				value.push_back(consumer.eat());
				if (value[0] == 't') {
					std::string test = "rue";
					for (int i = 0; i < 3; ++i) {
						if (consumer.peek() != test[i]) {
							LOG_SOFT_ERROR("Failed to parse JSON tree node, Unknown keyword; could be true");
							error = true;
							return false;
						}
						value.push_back(consumer.eat());
					}
				}
				else {
					std::string test = "alse";
					for (int i = 0; i < 4; ++i) {
						if (consumer.peek() != test[i]) {
							LOG_SOFT_ERROR("Failed to parse JSON tree node, Unknown keyword; could be false");
							error = true;
							return false;
						}
						value.push_back(consumer.eat());
					}
				}
				return true;
			}
			bool JSON::IsNullValid(std::string& value, Consumer& consumer, bool& error) {
				if (consumer.peek() != 'n') return false;
				value.push_back(consumer.eat());
				std::string test = "ull";
				for (int i = 0; i < 3; ++i) {
					if (consumer.peek() != test[i]) {
						LOG_SOFT_ERROR("Failed to parse JSON tree node, Unknown keyword; could be null");
						error = true;
						return false;
					}
					value.push_back(consumer.eat());
				}

				return true;
			}
		}
	}
}