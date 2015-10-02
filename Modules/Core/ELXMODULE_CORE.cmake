set( DOCUMENTATION 
  "This module contains the core components of elastix library such as the component database and component pipeline instantiation functionality."
)

elxmodule_enable( ELXMODULE_CORE
  DEPENDS
    ELXMODULE_COMMON
  DESCRIPTION
    "${DOCUMENTATION}"
)
