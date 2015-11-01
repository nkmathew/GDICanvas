/*!
 * \file Colors.h
 * \brief Provides color name conversion functions.
 */

#ifndef COLORNAMES_H

#define COLORNAMES_H

//! The total number of colornames available
#define COLORNAMES 538

#include <windows.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>

/*!
 * \namespace Colors
 *
 * Contains color conversion functions
 */
namespace Colors {

/*!
 * \struct RGBName
 * \brief Represents a color's properties
 */
struct RGBName {
  //! Red, Green and Blue values of the color
  char rgbValue[10];
  //! The hex represention of the color
  char colorName[25];
};

//! Represents the components of an RGB color specification
struct RGBValue {
  double red, green, blue;
};

/*!
 * \brief Returns the hex represention of a known color
 *
 * \param[in] colorName The name of the color. It need not be in any particular
 * case. The only requirement(for simplicity sake) is that there be no space
 * in the string. Much like CSS/HTML color names.
 *
 * \code
 *    std::cout << hexValue("Coral"); // prints #FF7F50
 * \endcode
 * \note
 */
std::string hexValue(const std::string &colorName);

/*!
 * \brief Converts a hex color string to its RGB represention.
 *
 * \returns A struct `{red, green, blue}` representing the color
 * \param[in] colorString The hex digits representing the color.
 *            The string must be at least 1 character long and at most 6.
 * \note
 *    A hex string with less than six characters will generate a color whose
 *    hex notation has the last character in the string repeated.
 *    e.g
 *      \code
 *         hexToRGB("#555"); // {85, 85, 85}, string used: "#555555"
 *         hexToRGB("#ABC"); // {171, 204, 204}, string used: "#ABCCCC"
 *         hexToRGB("#3"); // {51, 51, 51}, string used: "#333333"
 *         hexToRGB("#ZZZZZ"); // {255, 255, 255}
 *      \endcode
 */
RGBValue hexToRGB(const std::string &colorString);

/*!
 * \brief Checks if a color string has valid hexadecimal characters and tries to
 * fix acceptably valid ones.
 *
 * A hex string will less than six characters will have the last character
 * repeated till the hex digits become 6.
 *
 * \returns An empty string on invalid hex string.
 *
 * \code
 *   fixHexString("#A"); // "#AAAAAA"
 *   fixHexString("#AFCD"); // "#AFCDDD"
 *   fixHexString("#AF#CD"); // "#AFCDDD"
 *   fixHexString("ACCDDE"); // "#ACCDDE"
 *   fixHexString("grey"); // ""
 * \endcode
 *
 */
std::string fixHexString(const std::string &colorString);

//! Converts the color in hex form to an integer used by all GDI functions
COLORREF hexToColorRef(const std::string &colorString);

/*!
 * \brief Converts the rgb color specification to a hexadecimal form
 */
std::string RGBTohex(int red, int green, int blue);

} // namespace Colors

#endif
