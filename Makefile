all: default

default:
	#sed -i -e 's/\r$//' compile
	chmod 755 compile
	./compile
	#touch cSpec.h
	#cat "#ifndef __CSPEC_H_" > cSpec.h
	#cat "#define __CSPEC_H_" >> cSpec.h
	#cat src/helpers.h >> cSpec.h
	#cat src/data_structures.h >> cSpec.h
	#cat src/functionality.h >> cSpec.h
	#cat src/interface.h >> cSpec.h
	#cat src/assert_modules/charptr_array_assert.h >> cSpec.h
	#cat src/assert_modules/charptr_assert.h >> cSpec.h
	#cat src/assert_modules/double_array_assert.h >> cSpec.h
	#cat src/assert_modules/double_assert.h >> cSpec.h
	#cat src/assert_modules/expression_assert.h >> cSpec.h
	#cat src/assert_modules/int_array_asserrt.h >> cSpec.h
	#cat src/assert_modules/int_assert.h >> cSpec.h
	#cat "#endif" >> cSpec.h
	#$(RM) -r export
	#mkdir export
	#mv cSpec.h export/

lib: default

test:
	@echo

spec: test

clean:
	$(RM) -r export
