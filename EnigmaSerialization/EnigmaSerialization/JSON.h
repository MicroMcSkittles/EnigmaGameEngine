#pragma once
#include <Enigma/Core/Core.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>

namespace Enigma {
	namespace Serialization {
		namespace JSON {

			enum class DataTreeType {
				String,
				Int,
				Float,
				Array,
				Object,
				Boolean,
				Null
			};

			enum DataTreeFormatFlag {
				None    = 0,
				NewLine         = BIT(0), // Use new line chars in node
				Indent          = BIT(1), // Indent node
				CarryToChildren = BIT(2), // Give all child nodes the same flags as the parent
				Collapse        = BIT(3)
			};
			// Used to format JSON data trees when saving
			struct DataTreeFormat {
				// Serialization format flags
				uint8_t flags = NewLine | Indent;
				// Specifies the order of an objects children in the JSON file, Only effects object nodes
				std::vector<std::string> childOrder;
			};

			struct DataTreeValue {
				DataTreeType type = DataTreeType::Null;
				std::string value;

				bool IsNull();
				operator int();
				operator float();
				operator bool();
				operator std::string();
			};

			struct DataTreeNode {
				DataTreeValue value;
				std::vector<DataTreeNode> elements;
				std::map<std::string, DataTreeNode> children;
				DataTreeFormat format;

				DataTreeNode() { }
				DataTreeNode(DataTreeType type) : value({ type, "" }) { }
				DataTreeNode(int v);
				DataTreeNode(float v);
				DataTreeNode(bool v);
				DataTreeNode(const std::string& v);
				DataTreeNode(const char* v);

				// If DataTree is an array, At will return the node At id in the array
				DataTreeNode& At(int id);

				DataTreeNode& operator[] (const std::string& name);
				const DataTreeNode& operator[] (const std::string& name) const;
			};

			class JSON {
			public:
				// Loads a JSON data tree from file, returns false if failed to load
				static bool LoadFile(const std::string& filename, DataTreeNode* tree);
				// Creates JSON data tree from JSON source code, returns false if failed to parse
				static bool ParseSource(const std::string& source, DataTreeNode* tree);

				// Saves a JSON data tree to file, returns false if failed to save
				// You must specify the order of child nodes in object nodes with the nodes format
				static bool Serialize(const std::string& filename, DataTreeNode* tree);

			private:
				struct Consumer {
					std::string source;
					std::stringstream buffer;

					// Returns the char at the buffers pointer, then increments the pointer
					char eat();
					// Returns the char at the buffers pointer
					char peek();
					// Pushes c to the buffer
					void push(char c);
				};

			private:
				// Creates a string with tabDepth number of Tabs
				static std::string Tab(int tabDepth);

				// Saves object node to file
				static void SerializeObject(std::ofstream& file, DataTreeNode* tree, int tabDepth);
				// Saves array node to file
				static void SerializeArray(std::ofstream& file, DataTreeNode* tree, int tabDepth);

				static void SerializeString(std::ofstream& file, const std::string& str);

				// Removes all white spaces from consumers source/buffer
				static bool RemoveWhiteSpace(Consumer& consumer);

				static bool IsObjectValid(Consumer& consumer, DataTreeNode* tree, bool& error);
				static bool IsArrayValid(Consumer& consumer, DataTreeNode* tree, bool& error);

				static bool IsStringValid(std::string& value, Consumer& consumer, bool& error);
				static bool IsNumberValid(std::string& value, DataTreeType& type, Consumer& consumer, bool& error);
				static bool IsBooleanValid(std::string& value, Consumer& consumer, bool& error);
				static bool IsNullValid(std::string& value, Consumer& consumer, bool& error);
			};
		}
	}
}