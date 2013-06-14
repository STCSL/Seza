set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(SOURCES
    main.cpp
)


set(HEADERS 
    ${HEADER_PATH}/JsonDefinitions.h
    ${HEADER_PATH}/JsonDeserializer.h
	${HEADER_PATH}/JsonSerializer.h
	${HEADER_PATH}/Seza.h
)

add_executable (JsonExample ${SOURCES} ${HEADERS})