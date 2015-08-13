list( APPEND ElastixRequiredITKModules
  ITKReview
)

foreach( ITKModule ${ElastixRequiredITKModules} )
  if( NOT ${ITKModule}_LOADED )
    message( FATAL_ERROR "Elastix requires that ITK is build with ${ITKModule}. Please rebuild ITK with Module_${ITKModule} set to ON." )
  endif()
endforeach()
