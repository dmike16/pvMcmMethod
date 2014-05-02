/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                                                                          *
 *  PVMETHOD-PROJECT                                                        *
 *                                                                          *
 *  ================                                                        *
 *                                                                          *
 *  This my thesis's project, about a method to approximate                 *
 *  the equation of surface evolution by mean curvature flow in R^3 space.  *
 *  At the end of the execution of this program we'll have an octave script *
 *  in order to plot the solution.                                          *
 *  Usage: pvschema options <file_initial_values>.dat                       *
 *  -h  --help          			Display usage information   *
 *  -s  --sphere        			set IC -> Sphere            *
 *  -t  --torus         			set IC -> Torus             *
 *  -d  --dumbbell      		  	set IC -> Dumbbell          *
 *  -n  --smooth-noise filename 	        Smooth an image with noise  *
 *  ===========================                                             *
 *                                                                          *
 *  The <file_initial_values>.dat must contain only the initial values in   *
 *  numerical form, in this order (see example ini.dat):                    *
 *   .OutPutTime                                                            *
 *   .DimSpace (that must be 3 to use the func in pvmcm directory)          *
 *   .Nodes                                                                 *
 *   .Radius                                                                *
 *   [.Radius] (In the case there are more than one radius)                 * 
 *   .Level-set                                        			    *
 *   (for DimSpace times)                                                   *
 *   .LowAxesBoundary                                                       *
 *   .UpperAxesBoundary                                                     *
 *                                                                          *
 *  ===================                                                     *
 * Example 'ini.dat':                                                       *
 * 0.5                                                                      *
 * 3                                                                        *
 * 100                                                                      *
 * 3.00                                                                     *
 * 0.5                                                                      *
 * -4.00                                                                    *
 * 4.00                                                                     *
 * -4.00                                                                    *
 * 4.00                                                                     *
 * -4.00                                                                    *
 * 4.00                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */