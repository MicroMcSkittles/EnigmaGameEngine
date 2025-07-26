#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>

// W.I.P. I plan on changing basically everything in this file

namespace Enigma {
	namespace Serialization {
		// Fixed point number, apple is strange
		typedef uint32_t Fixed;
		float FixedToFloatingPoint(Fixed v);

		struct GlyphData {
			short numberOfContours;
			short minX;
			short minY;
			short maxX;
			short maxY;

			std::vector<uint16_t> endPointsOfContours;
			std::vector<uint8_t> instructions;

			int offsetCount;

			std::vector<uint8_t> offsetFlags;

			std::vector<int> offsetsX;
			std::vector<int> offsetsY;

			bool isCompound;
			uint16_t compoundFlags;
		};

		struct FontHead {
			Fixed version;
			Fixed fontRevision;
			uint32_t checkSumAdjustment;
			uint32_t magicNumber; // ?
			uint16_t flags;
			uint16_t unitsPerEm;
			uint64_t created;
			uint64_t modified;
			short minX;
			short minY;
			short maxX;
			short maxY;
			uint16_t macStyle;
			uint16_t lowestRecPPEM;
			short fontDirectionHint;
			short indexToLocFormat;
			short glyphDataFormat;
		};
		struct FontDirectory {
			uint32_t scalerType;
			uint16_t tableCount;
			uint16_t searchRange;
			uint16_t entrySelector;
			uint16_t rangeShift;
		};
		struct FontTableDirectory {
			uint32_t tag;
			uint32_t checkSum;
			uint32_t offset;
			uint32_t length;

			std::string GetTagStr();
		};
		struct MaxComponentElements {
			Fixed version;
			uint16_t glyphCount;
		};
		struct GlyphLocationTable {
			std::vector<uint32_t> offsets;
			std::vector<uint32_t> lengths;
		};

		struct CharMapSubTable {
			uint16_t platformID;
			uint16_t platformSpecificID;
			uint32_t offset;

			// format 4 TODO: support all formats, why are there so many? :(
			uint16_t format;
			uint16_t length;
			uint16_t language;
			uint16_t segCountX2;
			uint16_t searchRange;
			uint16_t entrySelector;
			uint16_t rangeShift;
			std::vector<uint16_t> endCodes;
			uint16_t reservedPad;
			std::vector<uint16_t> startCodes;
			std::vector<uint16_t> idDeltas;
			std::vector<uint16_t> idRangeOffsets;
			std::vector<uint16_t> glyphIndexArray;

			uint16_t Search(uint16_t unicode);
		};
		struct CharMapTable {
			uint16_t version;
			uint16_t subTableCount;

			std::vector<CharMapSubTable> subTables;
		};

		/*class Font {
		public:
			Font(const std::string& filename);
			~Font();

			Glyph GetGlyph(uint16_t unicode);

		private:

		};*/
		class Font {
		public:
			Font(const std::string& filename);
			~Font();
	
		private:
			void ReadFontHeadTable(std::ifstream& file);
			void ReadMaxComponentElements(std::ifstream& file);
			void ReadGlyphLocationTable(std::ifstream& file);
			void ReadCharMapTable(std::ifstream& file);

			void ReadCompoundGlyph(std::ifstream& file, GlyphData& data);
			void ReadSimpleGlyph(std::ifstream& file, GlyphData& data);
			void ReadGlyphOffsets(std::ifstream& file, GlyphData& data, bool xOffset);
			void ReadGlyphData(std::ifstream& file);

		// Private is commented out to make debuging easier
		//private:
		public:
			FontHead m_FontHead;
			FontDirectory m_FontDirectory;
			MaxComponentElements m_MaxComponentElements;
			std::map<std::string, FontTableDirectory> m_Tables;
			std::vector<GlyphData> m_GlyphData;
			GlyphLocationTable m_GlyphLocationTable;
			CharMapTable m_CharMapTable;
		};
	}
}