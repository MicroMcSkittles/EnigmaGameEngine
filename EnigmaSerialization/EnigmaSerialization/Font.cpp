#include "Font.h"
#include "Utils.h"
#include <fstream>
#include <Enigma/Core/Core.h>

namespace Enigma {
	namespace Serialization {

		float FixedToFloatingPoint(Fixed v)
		{
			uint16_t intHalf = (v >> 16) & 0xFFFF;
			uint16_t fracHalf = v & 0xFFFF;
			
			float rslt = (float)intHalf + (1.0f / (float)fracHalf);

			return rslt;
		}

		std::string FontTableDirectory::GetTagStr()
		{
			std::string str;
			str.resize(4);
			str[3] = (char)(tag & 0xFF);
			str[2] = (char)((tag >> 8) & 0xFF);
			str[1] = (char)((tag >> 16) & 0xFF);
			str[0] = (char)((tag >> 24) & 0xFF);
			return str;
		}

		Font::Font(const std::string& filename)
		{
			std::ifstream file;
			file.open(filename, std::ios::binary);
			if (!file.is_open()) {
				LOG_SOFT_ERROR("Failed to open font file ( %s )", filename.c_str());
				return;
			}

			m_FontDirectory.scalerType    = ReadU32(file, false);
			m_FontDirectory.tableCount    = ReadU16(file, false);
			m_FontDirectory.searchRange   = ReadU16(file, false);
			m_FontDirectory.entrySelector = ReadU16(file, false);
			m_FontDirectory.rangeShift    = ReadU16(file, false);

			std::cout << "Scaler Type: " << m_FontDirectory.scalerType << std::endl;
			std::cout << "Table Count: " << m_FontDirectory.tableCount << std::endl;
			std::cout << "Search Range: " << m_FontDirectory.searchRange << std::endl;
			std::cout << "Entry Selector: " << m_FontDirectory.entrySelector << std::endl;
			std::cout << "Range Shift: " << m_FontDirectory.rangeShift << std::endl;

			for (int i = 0; i < m_FontDirectory.tableCount; ++i) {
				FontTableDirectory table;
				table.tag      = ReadU32(file, false);
				table.checkSum = ReadU32(file, false);
				table.offset   = ReadU32(file, false);
				table.length   = ReadU32(file, false);
				std::string tag = table.GetTagStr();
				m_Tables.insert({ tag, table });

				std::cout << std::endl << i << ") " << tag << ":\n";
				std::cout << "Check Sum: " << table.checkSum << std::endl;
				std::cout << "Offset: " << table.offset << std::endl;
				std::cout << "Length: " << table.length << std::endl;
			}
			std::cout << std::endl;

			file.seekg(m_Tables["head"].offset);
			ReadFontHeadTable(file);

			file.seekg(m_Tables["maxp"].offset);
			ReadMaxComponentElements(file);

			file.seekg(m_Tables["loca"].offset);
			ReadGlyphLocationTable(file);

			file.seekg(m_Tables["cmap"].offset);
			ReadCharMapTable(file);

			for (int i = 0; i < m_MaxComponentElements.glyphCount - 1; ++i) {
				uint32_t offset = m_Tables["glyf"].offset;
				offset += m_GlyphLocationTable.offsets[i];
				file.seekg(offset);
				ReadGlyphData(file);
			}

			file.close();
		}
		Font::~Font()
		{
		}

		void Font::ReadFontHeadTable(std::ifstream& file)
		{
			m_FontHead.version            = ReadU32(file, false);
			m_FontHead.fontRevision       = ReadU32(file, false);
			m_FontHead.checkSumAdjustment = ReadU32(file, false);
			m_FontHead.magicNumber        = ReadU32(file, false);
			m_FontHead.flags              = ReadU16(file, false);
			m_FontHead.unitsPerEm         = ReadU16(file, false);
			m_FontHead.created            = ReadU64(file, false);
			m_FontHead.modified           = ReadU64(file, false);
			m_FontHead.minX               = ReadS16(file, false);
			m_FontHead.minY               = ReadS16(file, false);
			m_FontHead.maxX               = ReadS16(file, false);
			m_FontHead.maxY               = ReadS16(file, false);
			m_FontHead.macStyle           = ReadU16(file, false);
			m_FontHead.lowestRecPPEM      = ReadU16(file, false);
			m_FontHead.fontDirectionHint  = ReadS16(file, false);
			m_FontHead.indexToLocFormat   = ReadS16(file, false);
			m_FontHead.glyphDataFormat    = ReadS16(file, false);
		}
		void Font::ReadMaxComponentElements(std::ifstream& file)
		{
			m_MaxComponentElements.version = ReadU32(file, false);
			m_MaxComponentElements.glyphCount = ReadU16(file, false);
			uint16_t maxPoints = ReadU16(file, false);
			uint16_t maxContours = ReadU16(file, false);
			uint16_t maxCompPoints = ReadU16(file, false);
		}
		void Font::ReadGlyphLocationTable(std::ifstream& file)
		{
			//std::cout << std::endl;
			m_GlyphLocationTable.offsets.resize(m_MaxComponentElements.glyphCount);
			m_GlyphLocationTable.lengths.resize(m_MaxComponentElements.glyphCount);
			for (int i = 0; i < m_MaxComponentElements.glyphCount; ++i) {
				uint32_t offset;
				if (m_FontHead.indexToLocFormat == 1) {
					offset = ReadU32(file, false);
				}
				else {
					offset = (uint32_t)ReadU16(file, false);
					offset *= 2;
				}
				m_GlyphLocationTable.offsets[i] = offset;
				if (i != 0) {
					uint32_t lastOffset = m_GlyphLocationTable.offsets[i - 1];
					m_GlyphLocationTable.lengths[i - 1] = offset - lastOffset;
				}
				//std::cout << "Glyph " << i << " ), Offset: " << offset << std::endl;
			}
		}
		void Font::ReadCharMapTable(std::ifstream& file)
		{
			m_CharMapTable.version = ReadU16(file, false);
			m_CharMapTable.subTableCount = ReadU16(file, false);

			std::cout << "Char Map Table\n";
			std::cout << "version: " << m_CharMapTable.version << std::endl;
			std::cout << "Sub Table Count: " << m_CharMapTable.subTableCount << std::endl << std::endl;

			uint32_t cmapOffset = m_Tables["cmap"].offset;
			m_CharMapTable.subTables.resize(m_CharMapTable.subTableCount);
			for (int i = 0; i < m_CharMapTable.subTableCount; ++i) {
				CharMapSubTable& subTable = m_CharMapTable.subTables[i];
				subTable.platformID = ReadU16(file, false);
				subTable.platformSpecificID = ReadU16(file, false);
				subTable.offset = ReadU32(file, false);
				//subTable.fontPath = filename;
			}

			for (int i = 0; i < m_CharMapTable.subTableCount; ++i) {
				CharMapSubTable& subTable = m_CharMapTable.subTables[i];
				uint32_t offset = cmapOffset + subTable.offset;
				file.seekg(offset);

				subTable.format = ReadU16(file, false);
				if (subTable.format != 4) continue;

				subTable.length = ReadU16(file, false);
				subTable.language = ReadU16(file, false);
				subTable.segCountX2 = ReadU16(file, false);
				subTable.searchRange = ReadU16(file, false);
				subTable.entrySelector = ReadU16(file, false);
				subTable.rangeShift = ReadU16(file, false);

				uint32_t segCount = subTable.segCountX2 / 2;

				subTable.endCodes.resize(segCount);
				for (uint32_t i = 0; i < segCount; ++i) {
					subTable.endCodes[i] = ReadU16(file, false);
				}

				subTable.reservedPad = ReadU16(file, false);

				subTable.startCodes.resize(segCount);
				for (uint32_t i = 0; i < segCount; ++i) {
					subTable.startCodes[i] = ReadU16(file, false);
				}

				subTable.idDeltas.resize(segCount);
				for (uint32_t i = 0; i < segCount; ++i) {
					subTable.idDeltas[i] = ReadU16(file, false);
				}

				subTable.idRangeOffsets.resize(segCount);
				for (uint32_t i = 0; i < segCount; ++i) {
					subTable.idRangeOffsets[i] = ReadU16(file, false);
				}

				subTable.glyphIndexArray.resize(m_MaxComponentElements.glyphCount);
				for (uint32_t i = 0; i < m_MaxComponentElements.glyphCount; ++i) {
					subTable.glyphIndexArray[i] = ReadU16(file, false);
				}

				std::cout << i << " )" << std::endl;
				std::cout << "Platform ID: " << subTable.platformID << std::endl;
				std::cout << "Platform Specific ID: " << subTable.platformSpecificID << std::endl;
				std::cout << "Offset: " << subTable.offset << std::endl;
				std::cout << "Length: " << subTable.length << std::endl;
				std::cout << "Language: " << subTable.language << std::endl;
				std::cout << "Segment Count: " << segCount << std::endl;
				std::cout << "Search Range: " << subTable.searchRange << std::endl;
				std::cout << "Entry Selector: " << subTable.entrySelector << std::endl;
				std::cout << "Range Shift: " << subTable.rangeShift << std::endl << std::endl;
			}
		}

		void Font::ReadGlyphOffsets(std::ifstream& file, GlyphData& data, bool xOffset)
		{
			uint8_t vectorBit = BIT(1 + (xOffset ? 0 : 1));
			uint8_t signBit = BIT(4 + (xOffset ? 0 : 1));
			std::vector<int>& offsets = (xOffset ? data.offsetsX : data.offsetsY);

			offsets.resize(data.offsetCount);
			offsets[0] = 0;
			for (int i = 0; i < data.offsetCount; ++i) {
				uint8_t flag = data.offsetFlags[i];
				offsets[i] = offsets[std::max(0, i - 1)];
				// Check if x is a signed or unsigned int
				if (flag & vectorBit) {
					uint8_t x = ReadU8(file);
					// See if x is positive or negative
					if (flag & signBit)
						offsets[i] += x;
					else
						offsets[i] -= x;
				}
				else {
					// See if the offset exists or not
					if (flag & signBit) {
						continue;
					}
					// store offset
					short x = ReadS16(file, false);
					offsets[i] += x;
				}
			}
		}
		void Font::ReadCompoundGlyph(std::ifstream& file, GlyphData& data)
		{

		}
		void Font::ReadSimpleGlyph(std::ifstream& file, GlyphData& data)
		{
			data.endPointsOfContours.resize(data.numberOfContours);
			for (int i = 0; i < data.numberOfContours; ++i) {
				data.endPointsOfContours[i] = ReadU16(file, false);
			}

			data.offsetCount = data.endPointsOfContours[data.endPointsOfContours.size() - 1] + 1;
			data.offsetFlags.resize(data.offsetCount);

			// Read instructions
			uint16_t instructionCount = ReadU16(file, false);
			data.instructions.resize(instructionCount);
			for (uint16_t i = 0; i < instructionCount; ++i) {
				data.instructions[i] = ReadU8(file);
			}

			// Read flags
			for (int i = 0; i < data.offsetCount; ++i) {
				uint8_t flag = ReadU8(file);
				data.offsetFlags[i] = flag;
				if (flag & BIT(3)) {
					uint8_t repeatCount = ReadU8(file);
					for (int j = 0; j < repeatCount; ++j) {
						if (i == data.offsetFlags.size() - 1) break;
						data.offsetFlags[++i] = flag;
					}
				}
			}

			// Read X values
			ReadGlyphOffsets(file, data, true);

			// Read Y values
			ReadGlyphOffsets(file, data, false);
		}
		void Font::ReadGlyphData(std::ifstream& file)
		{
			GlyphData data;

			// Read glyph description
			data.numberOfContours = ReadS16(file, false);
			data.minX = ReadS16(file, false);
			data.minY = ReadS16(file, false);
			data.maxX = ReadS16(file, false);
			data.maxY = ReadS16(file, false);

			data.isCompound = (data.numberOfContours <= 0);
			if (data.isCompound) ReadCompoundGlyph(file, data);
			else ReadSimpleGlyph(file, data);

			m_GlyphData.push_back(data);
		}
		
		uint16_t CharMapSubTable::Search(uint16_t unicode)
		{
			uint16_t firstIndex = (uint16_t)-1;
			for (uint16_t i = 0; i < segCountX2 / 2; ++i) {
				uint16_t endCode = endCodes[i];
				if (endCode == 0xFFFF) {
					return 0; // return missing char glyph id
				}
				if (endCode >= unicode) {
					firstIndex = i;
					break;
				}
			}
			if (firstIndex == (uint16_t)-1) {
				return 0; // return missing char glyph id
			}

			uint16_t startCode = startCodes[firstIndex];
			if (startCode <= unicode) {
				uint16_t idDelta = idDeltas[firstIndex];
				uint16_t idRangeOffset = idRangeOffsets[firstIndex];

				if (idRangeOffset == 0) {
					uint16_t glyphID = idDelta + unicode;
					return glyphID;
				}

				// Convert pointer stuff to a index into the glyph index array
				uint16_t index = (idRangeOffset / 2) + (unicode - startCode) + (firstIndex - segCountX2 / 2);
				uint16_t glyphIndex = glyphIndexArray[index];
				if (glyphIndex == 0) return 0;

				uint16_t glyphID = idDelta + glyphIndex;
				return glyphID;
			}

			return 0; // return missing char glyph id
		}
	}
}