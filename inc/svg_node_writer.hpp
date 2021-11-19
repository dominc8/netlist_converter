#ifndef SVG_NODE_WRITER_HPP
#define SVG_NODE_WRITER_HPP

#include "graph.hpp"
#include "node.hpp"
#include <stdio.h>

void write_svg_capacitor(FILE *f, const node &n);
void write_svg_resistor(FILE *f, const node &n);
void write_svg_inductor(FILE *f, const node &n);
void write_svg_voltage(FILE *f, const node &n);
void write_svg_current(FILE *f, const node &n);
void write_svg_ground(FILE *f, const node &n);
void write_svg_dot(FILE *f, const node &n);

#endif /* SVG_NODE_WRITER_HPP */

