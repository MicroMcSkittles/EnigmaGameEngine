#pragma once
#include "EnigmaSerialization/Fonts/Font.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace Enigma::Serialization {
	namespace TTF {
		// Fixed point number, apple is strange
		typedef uint32_t Fixed;
		typedef uint16_t Fixed2Dot14;
		float FixedToFloatingPoint(Fixed v);
		float ShortToFixed2Dot14(short value);
		float ReadSFixed2Dot14(std::ifstream& file, bool littleE = true);
		
		struct FontDirectory {
			uint32_t scalerType;
			uint16_t tableCount;
			uint16_t searchRange;
			uint16_t entrySelector;
			uint16_t rangeShift;

			static void Read(std::ifstream& file, FontDirectory& fontDirectory);
		};
		struct FontTableDirectory {
			uint32_t tag;
			uint32_t checkSum;
			uint32_t offset;
			uint32_t length;

			std::string GetTagStr();
			static void Read(std::ifstream& file, FontDirectory& fontDirectory, std::map<std::string, FontTableDirectory>& tables);
		};
		struct FontHead {
			Fixed    version;
			Fixed    fontRevision;
			uint32_t checkSumAdjustment;
			uint32_t magicNumber; // ?
			uint16_t flags;
			uint16_t unitsPerEm;
			uint64_t created;
			uint64_t modified;
			short    minX;
			short    minY;
			short    maxX;
			short    maxY;
			uint16_t macStyle;
			uint16_t lowestRecPPEM;
			short    fontDirectionHint;
			short    indexToLocFormat;
			short    glyphDataFormat;

			static void Read(std::ifstream& file, FontHead& fontHead);
		};
		struct MaxComponentElements {
			Fixed    version;
			uint16_t glyphCount;
			uint16_t maxPoints;
			uint16_t maxContours;
			uint16_t maxComponentPoints;
			uint16_t maxComponentContours;
			uint16_t maxZones;
			uint16_t maxTwilightPoints;
			uint16_t maxStorage;
			uint16_t maxFunctionDefs;
			uint16_t maxInstructionDefs;
			uint16_t maxStackElements;
			uint16_t maxSizeOfIntructions;
			uint16_t maxComponentElements;
			uint16_t maxComponentDepth;

			static void Read(std::ifstream& file, MaxComponentElements& maxComponentElements);
		};
		struct HorizontalHeader {
			Fixed    version;
			short    ascent;
			short    descent;
			short    lineGap;
			uint16_t advanceWidthMax;
			short    minLeftSideBearing;
			short    minRightSideBearing;
			short    xMaxExtent;
			short    caretSlopeRise;
			short    caretSlopeRun;
			short    caretOffset;
			short    metricDataFormat;
			uint16_t numOfLongHorMetrics;

			static void Read(std::ifstream& file, HorizontalHeader& horizontalHeader);
		};
		struct LongHorizontalMetric {
			uint16_t advanceWidth;
			short    leftSideBearing;
		};
		struct HorizontalMetrics {
			std::vector<LongHorizontalMetric> horizontalMetrics;
			std::vector<short>		          leftSideBearing;

			static void Read(std::ifstream& file, HorizontalMetrics& horizontalMetrics, HorizontalHeader& horizontalHeader, MaxComponentElements& maxComponentElements);
		};
		struct GlyphLocationTable {
			std::vector<uint32_t> offsets;
			std::vector<uint32_t> lengths;

			static void Read(std::ifstream& file, uint16_t glyphCount, FontHead& fontHead, GlyphLocationTable& glyphLocationTable);
		};

		struct CharMapSubTable {
			uint16_t platformID;
			uint16_t platformSpecificID;
			uint32_t offset;

			uint16_t format;
			void* formatTable;

			int bestFitScore; // Used to find the best sub table, the bigger the number the more desirable the table

			uint16_t Search(uint32_t unicode);
			static void Read(std::ifstream& file, CharMapSubTable& charMapSubTable);

			~CharMapSubTable() { if (formatTable != nullptr) delete formatTable; }
		};
		struct CharMapFormat0 {
			uint16_t length;
			uint16_t language;
			std::vector<uint8_t> glyphIndexArray;

			uint16_t Search(uint32_t unicode);
			static void Read(std::ifstream& file, CharMapFormat0* charMapFormat0);
		};
		struct CharMapFormat4 {
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

			uint16_t Search(uint32_t unicode);
			static void Read(std::ifstream& file, uint16_t glyphCount, CharMapFormat4* charMapFormat4);
		};
		struct CharMapFormat12 {
			struct Group {
				uint32_t startCharCode;
				uint32_t endCharCode;
				uint32_t startGlyphCode;
			};
			uint32_t length;
			uint32_t language;
			uint32_t nGroups;
			std::vector<Group> groups;

			uint16_t Search(uint32_t unicode);
			static void Read(std::ifstream& file, uint16_t glyphCount, CharMapFormat12* charMapFormat12);
		};
		struct CharMapTable {
			uint16_t version;
			uint16_t subTableCount;

			uint16_t bestFit;
			std::vector<CharMapSubTable> subTables;

			static void Read(std::ifstream& file, uint16_t glyphCount, CharMapTable& charMapTable);
			static void CalcBestFitScore(CharMapSubTable& subtable);

		private:
			// Arrays used to score subtables

			inline static int s_PlatformPriority[4] = { 
				100,	// Unicode
				25,		// Macintosh
				-100,	// Not suppost to be used
				50		// Microsoft
			};
			inline static int s_UnicodePriority[7] = {
				25,		// Version 1.0 semantics
				25,		// Version 1.1 semantics
				-100,	// ISO 10646 1993 semantics (deprecated)
				100,	// Unicode 2.0 or later semantics (BMP only)
				100,	// Unicode 2.0 or later semantics (non-BMP characters allowed)
				50,		// Unicode Variation Sequences
				0		// Last Resort
			};
			inline static int s_MicrosoftPriority[11] = {
				100,	// Symbol
				100,	// Unicode
				0,		// ShiftJIS
				0,		// PRC
				0,		// Big5
				0,		// Wansung
				0,		// Johab
				0,0,0,	// Reserved
				100		// Unicode full repertoire
			};
		};

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

			static void ReadOffsets(std::ifstream& file, GlyphData& data, bool xOffset);
			static void ReadCompound(std::ifstream& file, GlyphData& data, uint32_t glyphTableOffset, GlyphLocationTable& glyphLocationTable);
			static void ReadSimple(std::ifstream& file, GlyphData& data);
			static void Read(std::ifstream& file, GlyphData& data, uint32_t glyphTableOffset, GlyphLocationTable& glyphLocationTable);
		};

		class FontTTF : public Font {
		public:
			FontTTF(const std::string& filename);
			virtual ~FontTTF();

			virtual Renderer::Glyph GetGlyph(uint16_t unicode) override;
			virtual int GetVerticalSpacing() override;
			virtual int GetSpaceHorizontalAdvance() override;

		private: // Helpers
			void GoToTable(std::ifstream& file, const std::string& table);

		private:
			FontHead m_FontHead;
			FontDirectory m_FontDirectory;
			MaxComponentElements m_MaxComponentElements;
			HorizontalHeader m_HorizontalHeader;
			HorizontalMetrics m_HorizontalMetrics;

			std::map<std::string, FontTableDirectory> m_Tables;

			std::vector<GlyphData> m_GlyphData;
			GlyphLocationTable m_GlyphLocationTable;
			CharMapTable m_CharMapTable;
		};
	}
}