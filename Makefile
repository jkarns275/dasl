all:
	mkdir -p bin/
	mkdir -p build/
	flex -o build/lexer.cxx parse/lexer.l
	bison -o build/parser.cxx parse/parser.yy
	g++ -g build/parser.cxx build/lexer.cxx parse/parse_tree.cxx parse/interner.cxx -shared -fPIC -o build/libparse.o -Ibuild/ -Iparse/
	g++ -g parse/lex_file.cxx build/libparse.o -o bin/test_lexer -Ibuild/ -Iparse/ -flto

clean:
	rm -rf lexer.cxx
	rm -rf parser.cxx parser.hxx location.hh position.hh stack.hh
	rm -rf bin
