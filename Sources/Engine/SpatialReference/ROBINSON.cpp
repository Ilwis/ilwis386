/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
/* ProjectionRobinson
  Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE  24 Jun 97    9:56 am
*/

#include "Engine\SpatialReference\ROBINSON.H"
#include "Engine\Base\DataObjects\ERR.H"

ProjectionRobinson::ProjectionRobinson()
: ProjectionPtr()
{
}
#define V(C,z) (C.c0 + z * (C.c1 + z * (C.c2 + z * C.c3)))
#define DV(C,z) (C.c1 + z * (C.c2 + C.c2 + z * 3. * C.c3))
/* note: following terms based upon 5 deg. intervals in degrees. */
static struct COEFS {
	double c0, c1, c2, c3;
} X[] = {
{1,	-5.67239e-12,	-7.15511e-05,	3.11028e-06,        },
{0.9986,	-0.000482241,	-2.4897e-05,	-1.33094e-06, },
{0.9954,	-0.000831031,	-4.4861e-05,	-9.86588e-07, },
{0.99,	-0.00135363,	-5.96598e-05,	3.67749e-06,    },
{0.9822,	-0.00167442,	-4.4975e-06,	-5.72394e-06, },
{0.973,	-0.00214869,	-9.03565e-05,	1.88767e-08,    },
{0.96,	-0.00305084,	-9.00732e-05,	1.64869e-06,    },
{0.9427,	-0.00382792,	-6.53428e-05,	-2.61493e-06, },
{0.9216,	-0.00467747,	-0.000104566,	4.8122e-06,   },
{0.8962,	-0.00536222,	-3.23834e-05,	-5.43445e-06, },
{0.8679,	-0.00609364,	-0.0001139,	3.32521e-06,    },
{0.835,	-0.00698325,	-6.40219e-05,	9.34582e-07,    },
{0.7986,	-0.00755337,	-5.00038e-05,	9.35532e-07,  },
{0.7597,	-0.00798325,	-3.59716e-05,	-2.27604e-06, },
{0.7186,	-0.00851366,	-7.0112e-05,	-8.63072e-06, },
{0.6732,	-0.00986209,	-0.000199572,	1.91978e-05,  },
{0.6213,	-0.010418,	8.83948e-05,	6.24031e-06,    },
{0.5722,	-0.00906601,	0.000181999,	6.24033e-06,  },
{0.5322, 0.,0.,0.  } },
Y[] = {
{0,	0.0124,	3.72529e-10,	1.15484e-09,            },
{0.062,	0.0124001,	1.76951e-08,	-5.92321e-09,   },
{0.124,	0.0123998,	-7.09668e-08,	2.25753e-08,    },
{0.186,	0.0124008,	2.66917e-07,	-8.44523e-08,   },
{0.248,	0.0123971,	-9.99682e-07,	3.15569e-07,    },
{0.31,	0.0124108,	3.73349e-06,	-1.1779e-06,      },
{0.372,	0.0123598,	-1.3935e-05,	4.39588e-06,    },
{0.434,	0.0125501,	5.20034e-05,	-1.00051e-05,   },
{0.4968,	0.0123198,	-9.80735e-05,	9.22397e-06,    },
{0.5571,	0.0120308,	4.02857e-05,	-5.2901e-06,    },
{0.6176,	0.0120369,	-3.90662e-05,	7.36117e-07,    },
{0.6769,	0.0117015,	-2.80246e-05,	-8.54283e-07,   },
{0.7346,	0.0113572,	-4.08389e-05,	-5.18524e-07,   },
{0.7903,	0.0109099,	-4.86169e-05,	-1.0718e-06,    },
{0.8435,	0.0103433,	-6.46934e-05,	5.36384e-09,    },
{0.8936,	0.00969679,	-6.46129e-05,	-8.54894e-06,   },
{0.9394,	0.00840949,	-0.000192847,	-4.21023e-06,   },
{0.9761,	0.00616525,	-0.000256001,	-4.21021e-06,   },
{1., 0.,0.,0 }                                    };
#define FXC	0.8487
#define FYC	1.3523
#define C1	11.45915590261646417544
#define RC1	0.08726646259971647884
#define NODES	18
#define ONEEPS	1.000001
#define EPS	1e-8
 /* sphere */
XY ProjectionRobinson::xyConv(const PhiLam& pl) const
{
  XY xy;

	int i;
	double dphi;

	i = floor((dphi = fabs(pl.Phi)) * C1);
	if (i >= NODES) i = NODES - 1;
	dphi = (dphi - RC1 * i) * 180 / M_PI;
	xy.x = V(X[i], dphi) * FXC * pl.Lam;
	xy.y = V(Y[i], dphi) * FYC;
	if (pl.Phi < 0.) xy.y = -xy.y;
	return xy;
}
PhiLam ProjectionRobinson::plConv(const XY& xy) const
{
  PhiLam pl;
	int i;
	double t, t1;
	struct COEFS T;

	pl.Lam = xy.x / FXC;
	pl.Phi = fabs(xy.y / FYC);
	if (pl.Phi >= 1.) { /* simple pathologic cases */
		if (pl.Phi > ONEEPS) return pl;
		else {
			pl.Phi = xy.y < 0. ? -M_PI_2 : M_PI_2;
			pl.Lam /= X[NODES].c0;
		}
	} else { /* general problem */
		/* in Y space, reduce to table interval */
		for (i = floor(pl.Phi * NODES);;) {
			if (Y[i].c0 > pl.Phi) --i;
			else if (Y[i+1].c0 <= pl.Phi) ++i;
			else break;
		}
		T = Y[i];
		/* first guess, linear interp */
		t = 5. * (pl.Phi - T.c0)/(Y[i+1].c0 - T.c0);
		/* make into root */
		T.c0 -= pl.Phi;
		for (;;) { /* Newton-Raphson reduction */
			t -= t1 = V(T,t) / DV(T,t);
			if (fabs(t1) < EPS)
				break;
		}
		pl.Phi = (5 * i + t) * M_PI / 180;
		if (xy.y < 0.) pl.Phi = -pl.Phi;
		pl.Lam /= V(X[i], t);
	}
	return pl;
}





