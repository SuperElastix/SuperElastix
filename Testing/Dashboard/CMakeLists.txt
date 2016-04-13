# ---------------------------------------------------------------------
# Dashboard configuration

mark_as_advanced( SUPERELASTIX_DASHBOARD_SOURCE_DIR )
set( SUPERELASTIX_DASHBOARD_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Testing/Dashboard" )

mark_as_advanced( SUPERELASTIX_DASHBOARD_BINARY_DIR )
set( SUPERELASTIX_DASHBOARD_BINARY_DIR "${CMAKE_BINARY_DIR}/Testing/Dashboard" )

mark_as_advanced( SUPERELASTIX_DASHBOARD_LOGSTASH_CERTIFICATE )
set( SUPERELASTIX_DASHBOARD_LOGSTASH_CERTIFICATE "selxLogstash.crt" )

mark_as_advanced( SUPERELASTIX_DASHBOARD_LOGSTASH_RSA_PRIVATE_KEY )
set( SUPERELASTIX_DASHBOARD_LOGSTASH_RSA_PRIVATE_KEY "selxLogstash.key" )

mark_as_advanced( SUPERELASTIX_DASHBOARD_VOLUME )
if( NOT EXISTS "${SUPERELASTIX_DASHBOARD_VOLUME}" )
  set( ELASTIX_DASHBOARD_VOLUME "${SUPERELASTIX_DASHBOARD_BINARY_DIR}/Data" )
  file( MAKE_DIRECTORY "${SUPERELASTIX_DASHBOARD_VOLUME}" )
endif()

# Test/Benchmark results will be forwarded to this IP. You will probably
# have change this in order to match docker settings on your own machine. 
mark_as_advanced( SUPERELASTIX_DASHBOARD_IP )
set( SUPERELASTIX_DASHBOARD_IP "192.168.59.103" )

configure_file(
  ${SUPERELASTIX_DASHBOARD_SOURCE_DIR}/selxDashboard.yml.in
  ${SUPERELASTIX_DASHBOARD_BINARY_DIR}/selxDashboard.yml
)

configure_file(
  ${SUPERELASTIX_DASHBOARD_SOURCE_DIR}/selxLogstashForwarder.yml.in
  ${SUPERELASTIX_DASHBOARD_BINARY_DIR}/selxLogstashForwarder.yml
)

configure_file(
  ${SUPERELASTIX_DASHBOARD_SOURCE_DIR}/selxLogstash.conf.in
  ${SUPERELASTIX_DASHBOARD_BINARY_DIR}/Logstash/selxLogstash.conf
)

configure_file(
  ${SUPERELASTIX_DASHBOARD_SOURCE_DIR}/selxLogstashForwarder.json.in
  ${SUPERELASTIX_DASHBOARD_BINARY_DIR}/selxLogstashForwarder.json
)

# ---------------------------------------------------------------------
# SSL certificates
# Use the following OpenSSL command to generate a test certificate and key if needed
# openssl req -subj '/CN=*/' -x509 -batch -nodes -days 9999 -newkey rsa:2048 -keyout selxLogstash.key -out selxLogstash.crt
