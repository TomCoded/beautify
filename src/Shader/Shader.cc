//Shader.cc
//(C) 2002 Tom White

#include <Shader/Shader.h>

Shader * Shader::create(Renderer *) {}
Shader * Shader::clone() {};

//destructor
Shader::~Shader() {}

//I/O functions

istream& Shader::in(istream&) {}

ostream& Shader::out(ostream&) {}
