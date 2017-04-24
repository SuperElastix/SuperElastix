We found that pngtest.c should be excluded from libpng when linked to Niftyreg, because its main() function sometimes overrules the main() of a SuperElastix client application.

As Floris mailed to Marc, the creator of Niftyreg:

  From: Berendsen, F. (LKEB)
  Sent: Friday, April 21, 2017 4:58 PM
  To: 'm.modat (at) ucl.ac.uk'
  Cc: Dekker, C.E. (LKEB)
  Subject: Solving issue with pnglib

