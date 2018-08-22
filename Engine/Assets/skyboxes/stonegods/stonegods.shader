//		
//		Stone Gods Skybox By Techx 
//		http://techx.digitalarenas.com
//			10/30/2001
//
// For help with q3map_sun and other shader commands visit this page:
// http://qeradiant.com/manual/Q3AShader_Manual/ch03/pg3_1.htm

textures/skies/stonegods
{
	qer_editorimage env/stonegods/sgod_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	surfaceparm sky
	q3map_sun .8 .8 .8 90 350 40      
	q3map_surfacelight 100

        skyparms env/stonegods/sgod - -
}
