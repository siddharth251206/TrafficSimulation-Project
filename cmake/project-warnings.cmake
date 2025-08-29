if(MSVC)
  target_compile_options(${PROJECT_NAME}_warnings INTERFACE
    /permissive-  # Enforces standards conformance
    /W4           # All reasonable warnings
    /w14640       # Enable warning on thread unsafe static member initialization
    /w14242       # 'identifier': conversion from 'type1' to 'type1', possible loss of data
    /w14254       # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
    /w14263       # 'function': member function does not override any base class virtual member function
    /w14265       # 'classname': class has virtual functions, but destructor is not virtual
    /w14287       # 'operator': unsigned/negative constant mismatch
    /we4289       # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
    /w14296       # 'operator': expression is always 'boolean_value'
    /w14311       # 'variable': pointer truncation from 'type1' to 'type2'
    /w14545       # expression before comma evaluates to a function which is missing an argument list
    /w14546       # function call before comma missing argument list
    /w14547       # 'operator': operator before comma has no effect; expected operator with side-effect
    /w14549       # 'operator': operator before comma has no effect; did you intend 'operator'?
    /w14555       # expression has no effect; expected expression with side-effect
    /w14619       # pragma warning: there is no warning number 'number'
    /w14826       # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior
    /w14905       # wide string literal cast to 'LPSTR'
    /w14906       # string literal cast to 'LPWSTR'
    /w14928       # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
  )
else()
  target_compile_options(${PROJECT_NAME}_warnings INTERFACE
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -pedantic
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wdouble-promotion
    -Wformat=2

    # GCC-specific warnings 
    $<$<CXX_COMPILER_ID:GNU>:
      $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,6.0>:
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wnull-dereference
      >
      $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,7.0>:
        -Wduplicated-branches
      >
      $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,4.8>:
        -Wuseless-cast
      >
      -Wlogical-op
    >

    # Clang-specific warnings
    $<$<CXX_COMPILER_ID:Clang>:
      # Note: -Wimplicit-fallthrough is included in -Wextra for GCC but needs explicit enabling for Clang
      -Wimplicit-fallthrough
    >
  )
endif()
