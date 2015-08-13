# ---------------------------------------------------------------------
# Settings
mark_as_advanced( ELASTIX_DASHBOARD_HOST )
set( ELASTIX_DASHBOARD_HOST "elastix" )

mark_as_advanced( ELASTIX_DASHBOARD_ELASTICSEARCH_PORT )
set( ELASTIX_DASHBOARD_ELASTICSEARCH_PORT 9200 )

mark_as_advanced( ELASTIX_DASHBOARD_LOGSTASH_PORT )
set( ELASTIX_DASHBOARD_LOGSTASH_PORT 5043 )

mark_as_advanced( ELASTIX_DASHBOARD_KIBANA_PORT )
set( ELASTIX_DASHBOARD_KIBANA_PORT 5601 )

mark_as_advanced( ELASTIX_DASHBOARD_SOURCE_DIR )
set( ELASTIX_DASHBOARD_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Testing/Dashboard" )

mark_as_advanced( ELASTIX_DASHBOARD_BINARY_DIR )
set( ELASTIX_DASHBOARD_BINARY_DIR "${CMAKE_BINARY_DIR}/Testing/Dashboard" )

mark_as_advanced( ELASTIX_DASHBOARD_LOGSTASH_CERTIFICATE )
set( ELASTIX_DASHBOARD_LOGSTASH_CERTIFICATE "elxLogstashForwarder.crt" )

mark_as_advanced( ELASTIX_DASHBOARD_LOGSTASH_RSA_PRIVATE_KEY )
set( ELASTIX_DASHBOARD_LOGSTASH_RSA_PRIVATE_KEY "elxLogstashForwarder.key" )

mark_as_advanced( ELASTIX_DASHBOARD_VOLUME )
set( ELASTIX_DASHBOARD_VOLUME "${ELASTIX_DASHBOARD_BINARY_DIR}/Data" )

if( NOT EXISTS "${ELASTIX_DASHBOARD_VOLUME}" )
  message( STATUS
    "ELASTIX_DASHBOARD_VOLUME points to the default ${CMAKE_BINARY_DIR}/Testing/Dashboard/Data path in your build directory. Point to another directory on your local machine to ensure dashboard state is saved between builds."
  )

  file( MAKE_DIRECTORY "${ELASTIX_DASHBOARD_VOLUME}" )
endif()

# ---------------------------------------------------------------------
# Configure 
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/docker-compose.yml.in
  ${ELASTIX_DASHBOARD_BINARY_DIR}/docker-compose.yml
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/elxLogstash.conf.in
  ${ELASTIX_DASHBOARD_BINARY_DIR}/elxLogstash/elxLogstash.conf
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/elxLogstashForwarder.conf.in
  ${ELASTIX_DASHBOARD_BINARY_DIR}/elxLogstashForwarder.conf
)