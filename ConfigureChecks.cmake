##################################################


##################################################
# Our Proprietary Libraries
#####
FIND_PACKAGE(izenelib REQUIRED
  COMPONENTS
  febird
  am
  izene_util
  )
  
FIND_PACKAGE(wisekma REQUIRED)
FIND_PACKAGE(izenecma REQUIRED)
FIND_PACKAGE(ilplib REQUIRED COMPONENTS langid la)
FIND_PACKAGE(imllib REQUIRED)

SET(xml2_FOUND FALSE)

FIND_PACKAGE(xml2 REQUIRED)
IF(NOT xml2_FOUND)
	MESSAGE(FATAL_ERROR "cannot found libxml2.")
ENDIF(NOT xml2_FOUND)

##################################################
# Other Libraries
#####

FIND_PACKAGE(Threads REQUIRED)

SET(Boost_ADDITIONAL_VERSIONS 1.40 1.40.0 1.39 1.39.0 1.38 1.38.0 1.37 1.37.0 1.36 1.36.0)
FIND_PACKAGE(Boost 1.36 REQUIRED
  COMPONENTS
  system
  program_options
  thread
  regex
  date_time
  serialization
  filesystem
  unit_test_framework
  iostreams
  )

FIND_PACKAGE(TokyoCabinet 1.4.29 REQUIRED)
FIND_PACKAGE(Glog REQUIRED)

