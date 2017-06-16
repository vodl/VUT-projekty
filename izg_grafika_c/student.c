/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: student.c 217 2012-02-28 17:10:06Z spanel $
 *
 * xdlapa01@stud.fit.vutbr.cz
 */

#include "student.h"
#include "transform.h"

#include "render.h"

#include <memory.h>
#include <stdio.h>

#define APOW 1
//alfa mocnina lesklosti materialu


/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej */

S_Renderer * studrenCreate()
{
    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");

    /* inicializace default rendereru */
    renInit(&renderer->base);

    /* nastaveni ukazatelu na vase upravene funkce */
    renderer->base.projectTriangleFunc = studrenProjectTriangle;
	renderer->base.calcReflectanceFunc = studrenPhongReflectance; //phong

    return (S_Renderer *)renderer;
}

/****************************************************************************
 * Funkce vyhodnoti Phonguv osvetlovaci model pro zadany bod
 * a vraci barvu, kterou se bude kreslit do frame bufferu
 * point - souradnice bodu v prostoru
 * normal - normala k povrchu v danem bode */

S_RGBA studrenPhongReflectance(S_Renderer *pRenderer, const S_Coords *point, const S_Coords *normal)
{

	S_Coords    lvec, rvec, vvec;
    double      specular, diffuse, r, g, b;
    S_RGBA      color;

    IZG_ASSERT(pRenderer && point && normal);

    /* vektor ke zdroji svetla */
    lvec = makeCoords(pRenderer->light_position.x - point->x,
                      pRenderer->light_position.y - point->y,
                      pRenderer->light_position.z - point->z);
    coordsNormalize(&lvec);

	/* vektor z kamery do bodu */
	vvec = makeCoords(0 - point->x,
		              0 - point->y,
					  - pRenderer->camera_dist - point->z );
    coordsNormalize(&vvec);


    /* ambientni cast */ // Ca * Ia
    r = pRenderer->light_ambient.red   *  pRenderer->mat_ambient.red;
    g = pRenderer->light_ambient.green *  pRenderer->mat_ambient.green;
    b = pRenderer->light_ambient.blue  *  pRenderer->mat_ambient.blue;


    /* difuzni cast */ //Cd*Id*(NL) NL = diffuse
    diffuse = lvec.x * normal->x + lvec.y * normal->y + lvec.z * normal->z;
    if( diffuse > 0 )
    {
        r += diffuse * pRenderer->light_diffuse.red   *  pRenderer->mat_diffuse.red;
        g += diffuse * pRenderer->light_diffuse.green *  pRenderer->mat_diffuse.green;
        b += diffuse * pRenderer->light_diffuse.blue  *  pRenderer->mat_diffuse.blue;
    }

	/* odlesky */ // Cs * Is * (VR)^n
	//Vypocet odrazeneho paprsku: R = 2N(N*L)-L
	rvec.x = 2*normal->x * diffuse - lvec.x;
	rvec.y = 2*normal->y * diffuse - lvec.y;
	rvec.z = 2*normal->z * diffuse - lvec.z;
	//coordsNormalize(&rvec);

    //V.R
	specular = (rvec.x * vvec.x + rvec.y * vvec.y + rvec.z* vvec.z);
    if( specular > 0 )
    {
		r += pow(specular,APOW) * pRenderer->light_specular.red   *  pRenderer->mat_specular.red;
        g += pow(specular,APOW) * pRenderer->light_specular.green *  pRenderer->mat_specular.green;
        b += pow(specular,APOW) * pRenderer->light_specular.blue  *  pRenderer->mat_specular.blue;
    }
    
    /* saturace osvetleni*/
    r = MIN(1, r);
    g = MIN(1, g);
    b = MIN(1, b);

    /* kreslici barva */
    color.red = ROUND2BYTE(255 * r);
    color.green = ROUND2BYTE(255 * g);
    color.blue = ROUND2BYTE(255 * b);
    return color;
}




/****************************************************************************
 * Nova fce pro rasterizace trojuhelniku do frame bufferu
 * vcetne interpolace z souradnice a prace se z-bufferem
 * a interpolaci normaly pro Phonguv osvetlovaci model
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky */

void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         )
{
	int xMax, xMin, yMax, yMin; //lokalni extremy
	int a1, a2, a3, b1, b2, b3, c1, c2, c3; //koeficienty
	int Px, Py; // aktualni bod
	double Pz; //z-souradnice
	double lam1,lam2,lam3; // lambdy barycentrickych souradnic
	S_RGBA  color;  // barvy
	S_Coords np, pp;

    // vypocet extremu pro poradi cyklu
	xMax = MAX(x1,MAX(x2,x3));
	xMin = MIN(x1,MIN(x2,x3));
    yMax = MAX(y1,MAX(y2,y3));
	yMin = MIN(y1,MIN(y2,y3));

	// snizeni extremu do okna - osetreni zapisu mimo frame buffer
    yMin = MAX(yMin,0);
    yMax = MIN(yMax, pRenderer->frame_h - 1);
    xMin = MAX(xMin,0);
    xMax = MIN(xMax, pRenderer->frame_w - 1);

  // rovnice primek
  //  A(x)     +      B(y)     +       C                =     0
    a1 = y2 - y1;   b1 = x1 - x2;   c1 = x2*y1 - x1*y2;
    a2 = y3 - y2;   b2 = x2 - x3;   c2 = x3*y2 - x2*y3;
    a3 = y1 - y3;   b3 = x3 - x1;   c3 = x1*y3 - x3*y1;
  
    //iterace pres cely obdelnik oblasti
    for (Py = yMin; Py <= yMax; Py++)
    {
        for (Px = xMin; Px <= xMax; Px++)
        {
			// barycentricke vahy
   		    lam1 = (a2*Px + b2*Py + c2) / (double) (a2*x1 + b2*y1 + c2);
	        lam2 = (a3*Px + b3*Py + c3) / (double) (a3*x2 + b3*y2 + c3) ;
		    lam3 = (a1*Px + b1*Py + c1) / (double) (a1*x3 + b1*y3 + c1);

			// lezi bod v trojuhelniku?
		    if(lam1 >= 0 && lam2 >=0 && lam3 >=0)
		    {
		        Pz = v1->z*lam1 + v2->z*lam2 + v3->z*lam3; 
			     //dopocitani z-souradnice pomoci barycentrickych vah

		        if(Pz < pRenderer->depth_buffer[Px+(Py*pRenderer->frame_w)])
		        { //kontrola z-bufferu
			        pRenderer->depth_buffer[Px+(Py*pRenderer->frame_w)] = Pz;

				    //interpolace normal pro michani barvy
					np = makeCoords(n1->x*lam1 + n2->x*lam2 + n3->x*lam3,
						            n1->y*lam1 + n2->y*lam2 + n3->y*lam3,
						            n1->z*lam1 + n2->z*lam2 + n3->z*lam3
						);
				    //coordsNormalize(&np);
					
					// bod - souradnice jako double
					pp = makeCoords(v1->x*lam1 + v2->x*lam2 + v3->x*lam3,
						            v1->y*lam1 + v2->y*lam2 + v3->y*lam3,
						            v1->z*lam1 + v2->z*lam2 + v3->z*lam3
						);


					color = pRenderer->calcReflectanceFunc(pRenderer, &pp, &np);
		            PIXEL(pRenderer, Px, Py) = color;
					//vykresleni namichane barvy do aktualniho bodu
		        }
            }
        }
    }
}







/*****************************************************************************
 * Vykresli i-ty trojuhelnik modelu
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i)
{

    S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci ve 3D pred projekci */
    S_Coords    nn,n1,n2,n3;            /* normala trojuhelniku po transformaci */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;

    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles));

    /* z modelu si vytahneme trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);

    /* transformace vrcholu matici model */
    trTransformVertex(&aa, cvecGetPtr(pModel->vertices, triangle->v[0]));
    trTransformVertex(&bb, cvecGetPtr(pModel->vertices, triangle->v[1]));
    trTransformVertex(&cc, cvecGetPtr(pModel->vertices, triangle->v[2]));

    /* promitneme vrcholy trojuhelniku na obrazovku */
    trProjectVertex(&u1, &v1, &aa);
    trProjectVertex(&u2, &v2, &bb);
    trProjectVertex(&u3, &v3, &cc);

    /* transformace normaly trojuhelniku matici model */
    trTransformVector(&nn, cvecGetPtr(pModel->trinormals, triangle->n));

	trTransformVector(&n1, cvecGetPtr(pModel->normals, triangle->v[0]));
	trTransformVector(&n2, cvecGetPtr(pModel->normals, triangle->v[1]));
	trTransformVector(&n3, cvecGetPtr(pModel->normals, triangle->v[2]));

    /* normalizace normaly */
    coordsNormalize(&nn);

    coordsNormalize(&n1);
    coordsNormalize(&n2);
    coordsNormalize(&n3);


    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }

    /* rasterizace celeho trojuhelniku */
    studrenDrawTriangle(pRenderer,
                    &aa, &bb, &cc, &n1,&n2,&n3,
                    u1, v1, u2, v2, u3, v3
                    );

}

/*****************************************************************************
 *****************************************************************************/
