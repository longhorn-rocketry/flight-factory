#ifndef FLIGHT_FACTORY_IO_HPP
#define FLIGHT_FACTORY_IO_HPP

#include <iostream>

namespace ff {

/**
 * Stdout for Flight Factory. Supports formatting codes.
 *
 * # followed by a code indicates a format change. $ followed by a code
 * indicates a color change. See maps at the top for valid codes. Format
 * changes must preceed color changes. Multicharacter codes must be clasped in
 * curly brackets {}.
 *
 * @param k_data string to print
 */
void out(std::string k_data);

/**
 * Stdout with an appended newline. Supports formatting codes. See ff:out for
 * details.
 *
 * @param k_data string to print
 */
void outln(std::string k_data);

/**
 * Identical to outln but centers the printed line in the terminal.
 *
 * @param k_data string to print
 */
void outln_ctr(std::string k_data);

/**
 * Prints a horizontal rule.
 *
 * @param code formatting code
 * @param c    rule character
 */
void br(std::string code, char c);

} // namespace ff

#endif
