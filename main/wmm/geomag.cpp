// WMM geomag routine to calc mag field at a point

// #include <cstdio>
// #include <cstdlib>
// #include <cstring>
#include "geomag.h"
#include "wmm_2020.h"

#include <logdef.h>
#include <cmath>


#undef WMM_TEST

#ifdef WMM_Test

// Test Data
// Date(year) Height(km)  Lat(deg)  Lon(deg)  X(nT) Y(nT) Z(nT) H(nT) F(nT) I(deg)  D(deg)  GV(deg)
static double test_data[][12] = {
  {2020, 0, 80, 0, 6570.4, -146.3, 54606.0, 6572.0, 55000.1, 83.14, -1.28, -1.28},
  {2020, 0, 0, 120, 39624.3, 109.9, -10932.5, 39624.4, 41104.9, -15.42, 0.16, 0},
  {2020, 0, -80, 240, 5940.6, 15772.1, -52480.8, 16853.8, 55120.6, -72.2, 69.36, -50.64},
  {2020, 100, 80, 0, 6261.8, -185.5, 52429.1, 6264.5, 52802, 83.19, -1.70, -1.70},
  {2020, 100, 0, 120, 37636.7, 104.9, -10474.8, 37636.9, 39067.3, -15.55, 0.16, 0},
  {2020, 100, -80, 240, 5744.9, 14799.5, -49969.4, 15875.4, 52430.6, -72.37, 68.78, -51.22},
  {2022.5, 0, 80, 0, 6529.9, 1.1, 54713.4, 6529.9, 55101.7, 83.19, 0.01, 0.01},
  {2022.5, 0, 0, 120, 39684.7, -42.2, -10809.5, 39684.7, 41130.5, -15.24, -0.06, 0},
  {2022.5, 0, -80, 240, 6016.5, 15776.7, -52251.6, 16885.0, 54912.1, -72.09, 69.13, -50.87},
  {2022.5, 100, 80, 0, 6224.0, -44.5, 52527.0, 6224.2, 52894.5, 83.24, -0.41, -0.41},
  {2022.5, 100, 0, 120, 37694.0, -35.3, -10362.0, 37694.1, 39092.4, -15.37, -0.05, 0},
  {2022.5, 100, -80, 240, 5815.0, 14803.0, -49755.3, 15904.1, 52235.4, -72.27, 68.55, -51.47},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

void WMM_Model::geomag_test()
{
  ESP_LOGI(FNAME, "World Magnetic Model test\n");
  WMM_Model wmm(MAXORD, EPOCH, &(WMMCOF[0][0]));

  // Do tests
  double *data = test_data[0];
  while ( data[0] != 0.0 ) {
    // Date(year) Height(km)  Lat(deg)  Lon(deg)  X(nT) Y(nT) Z(nT) H(nT) F(nT) I(deg)  D(deg)  GV(deg)
    float alt = data[1];
    float dlat = data[2];
    float dlon = data[3];
    float time = data[0];
    float dec, dip, ti, gv, x1, y1, z1, h1;
    wmm.geomag(alt, dlat, dlon, time, &x1, &y1, &z1, &h1, &dec, &dip, &ti, &gv);
    // Compare
    ESP_LOGI(FNAME, "TestInput T%f\tH%f\tLat%f\tLon%f\t:\n", time, alt, dlat, dlon);
    ESP_LOGI(FNAME, "TestData  X%f\tY%f\tZ%f\tH%f\tF%f\tI%f\tD%f\tGV%f\n", data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11] );
    ESP_LOGI(FNAME, "geomag %2d X%f\tY%f\tZ%f\tH%f\tF%f\tI%f\tD%f\tGV%f\n", MAXORD, x1, y1, z1, h1, ti, dip, dec, gv );
    data += 12;
  };
}
#endif


/*************************************************************************/
WMM_Model::WMM_Model(int md, float epo, float* wmmcof)
{

  float* wmmdat = wmmcof;
  maxord = md;
  epoch = epo;
  
  // READ WORLD MAGNETIC MODEL SPHERICAL HARMONIC COEFFICIENTS
  c[0][0] = 0.0;
  cd[0][0] = 0.0;
  int n = 1, m = 0;
  while ( n <= maxord && m <= maxord ) {
    float gnm  = wmmdat[0];
    float hnm  = wmmdat[1];
    float dgnm = wmmdat[2];
    float dhnm = wmmdat[3];
    // printf("%d %d %f %f %f %f\n", n, m, gnm, hnm, dgnm, dhnm);

    c[m][n] = gnm;
    cd[m][n] = dgnm;
    if (m != 0)
    {
      c[n][m-1] = hnm;
      cd[n][m-1] = dhnm;
    }
    if ( m < n ) {
      m++;
    } else {
      n++;
      m = 0;
    }
    wmmdat += 4;
  }

  // CONVERT SCHMIDT NORMALIZED GAUSS COEFFICIENTS TO UNNORMALIZED
  *snorm = 1.0;
  fm[0] = 0.0;
  for (n=1; n<=maxord; n++)
    {
      *(snorm+n) = *(snorm+n-1)*(float)(2*n-1)/(float)n;
      int j = 2;
      for (int m=0, D1=1,D2=(n-m+D1)/D1; D2>0; D2--,m+=D1)
        {
          k[m][n] = (float)(((n-1)*(n-1))-(m*m))/(float)((2*n-1)*(2*n-3));
          if (m > 0)
            {
              float flnmj = (float)((n-m+1)*j)/(float)(n+m);
              *(snorm+n+m*13) = *(snorm+n+(m-1)*13)*sqrt(flnmj);
              j = 1;
              c[n][m-1] = *(snorm+n+m*13)*c[n][m-1];
              cd[n][m-1] = *(snorm+n+m*13)*cd[n][m-1];
            }
          c[m][n] = *(snorm+n+m*13)*c[m][n];
          cd[m][n] = *(snorm+n+m*13)*cd[m][n];
        }
      fn[n] = (float)(n+1);
      fm[n] = (float)n;
    }
  k[1][1] = 0.0;
}

bool WMM_Model::geomag(float alt, float glat, float glon, float time, float *bx, float *by, float *bz, float *bh, float *dec, float *dip, float *ti, float *gv)
{
  float dt = time - epoch;
  bool time_ok = true;
  if ( dt < 0.0 || dt > 5.0 )
  {
    time_ok = false;
    // WARNING - TIME EXTENDS BEYOND MODEL 5-YEAR LIFE SPAN
    // CONTACT NGDC FOR PRODUCT UPDATES:
    //         National Geophysical Data Center
    //         NOAA EGC/2
    //         325 Broadway
    //         Boulder, CO 80303 USA
    //         Attn: Manoj Nair or Stefan Maus
    //         Phone:  (303) 497-4642 or -6522
    //         Email:  Manoj.C.Nair@Noaa.Gov
    //         or
    //         Stefan.Maus@noaa.gov
    //         Web: http://www.ngdc.noaa.gov/geomag/WMM/
  }

  // INITIALIZE CONSTANTS
  pp[0] = 1.0;
  dp[0][0] = 0.0;
  constexpr float a = 6378.137;
  constexpr float b = 6356.7523142;
  constexpr float re = 6371.2;
  constexpr float a2 = a*a;
  constexpr float b2 = b*b;
  constexpr float c2 = a2-b2;
  constexpr float a4 = a2*a2;
  constexpr float b4 = b2*b2;
  constexpr float c4 = a4 - b4;
  constexpr float dtr = M_PI/180.0;
  
  float srlat = sin(glat*dtr);
  float crlat = cos(glat*dtr);
  float srlat2 = srlat*srlat;
  float crlat2 = crlat*crlat;


  // CONVERT FROM GEODETIC COORDS. TO SPHERICAL COORDS.
  float q = sqrt(a2-c2*srlat2);
  float q1 = alt*q;
  float q2 = ((q1+a2)/(q1+b2))*((q1+a2)/(q1+b2));
  float ct = srlat/sqrt(q2*crlat2+srlat2);
  float st = sqrt(1.0-(ct*ct));
  float r2 = (alt*alt)+2.0*q1+(a4-c4*srlat2)/(q*q);
  float r = sqrt(r2);
  float d = sqrt(a2*crlat2+b2*srlat2);
  float ca = (alt+d)/r;
  float sa = c2*crlat*srlat/(r*d);

  sp[0] = 0.0;
  cp[0] = 1.0;
  sp[1] = sin(glon*dtr);
  cp[1] = cos(glon*dtr);
  for (int m=2; m<=maxord; m++)
  {
    sp[m] = sp[1]*cp[m-1]+cp[1]*sp[m-1];
    cp[m] = cp[1]*cp[m-1]-sp[1]*sp[m-1];
  }

  float aor = re/r;
  float ar = aor*aor;
  float br=.0, bt=.0, bp=.0, bpp=.0;
  for (int n=1; n<=maxord; n++)
    {
      ar = ar*aor;
      for (int m=0,D3=1,D4=(n+m+D3)/D3; D4>0; D4--,m+=D3)
        {
          //  COMPUTE UNNORMALIZED ASSOCIATED LEGENDRE POLYNOMIALS
          //  AND DERIVATIVES VIA RECURSION RELATIONS
          if (n == m)
          {
            *(snorm+n+m*13) = st* *(snorm+n-1+(m-1)*13);
            dp[m][n] = st*dp[m-1][n-1]+ct**(snorm+n-1+(m-1)*13);
          }
          else if (n == 1 && m == 0)
          {
            *(snorm+n+m*13) = ct* *(snorm+n-1+m*13);
            dp[m][n] = ct*dp[m][n-1]-st* *(snorm+n-1+m*13);
          }
          else if (n > 1 && n != m)
          {
            if (m > n-2) *(snorm+n-2+m*13) = 0.0;
            if (m > n-2) dp[m][n-2] = 0.0;
            *(snorm+n+m*13) = ct* *(snorm+n-1+m*13)-k[m][n]* *(snorm+n-2+m*13);
            dp[m][n] = ct*dp[m][n-1] - st* *(snorm+n-1+m*13)-k[m][n]*dp[m][n-2];
          }

          // TIME ADJUST THE GAUSS COEFFICIENTS
          tc[m][n] = c[m][n]+dt*cd[m][n];
          if (m != 0) tc[n][m-1] = c[n][m-1]+dt*cd[n][m-1];

          // ACCUMULATE TERMS OF THE SPHERICAL HARMONIC EXPANSIONS
          float par = ar* *(snorm+n+m*13);
          float temp1, temp2;
          if (m == 0)
          {
            temp1 = tc[m][n]*cp[m];
            temp2 = tc[m][n]*sp[m];
          }
          else
          {
            temp1 = tc[m][n]*cp[m]+tc[n][m-1]*sp[m];
            temp2 = tc[m][n]*sp[m]-tc[n][m-1]*cp[m];
          }
          bt = bt-ar*temp1*dp[m][n];
          bp += (fm[m]*temp2*par);
          br += (fn[n]*temp1*par);

          // SPECIAL CASE:  NORTH/SOUTH GEOGRAPHIC POLES
          if (st == 0.0 && m == 1)
          {
            if (n == 1) pp[n] = pp[n-1];
            else pp[n] = ct*pp[n-1]-k[m][n]*pp[n-2];
            float parp = ar*pp[n];
            bpp += (fm[m]*temp2*parp);
          }
        }
    }
  if (st == 0.0) bp = bpp;
  else bp /= st;

  // ROTATE MAGNETIC VECTOR COMPONENTS FROM SPHERICAL TO
  // GEODETIC COORDINATES
  *bx = -bt*ca-br*sa;
  *by = bp;
  *bz = bt*sa-br*ca;

  // COMPUTE DECLINATION (DEC), INCLINATION (DIP) AND
  // TOTAL INTENSITY (TI)
  *bh = sqrt((*bx * *bx)+(*by * *by));
  *ti = sqrt((*bh * *bh)+(*bz * *bz));
  *dec = atan2(*by,*bx)/dtr;
  *dip = atan2(*bz,*bh)/dtr;

  // COMPUTE MAGNETIC GRID VARIATION IF THE CURRENT
  // GEODETIC POSITION IS IN THE ARCTIC OR ANTARCTIC
  // (I.E. GLAT > +55 DEGREES OR GLAT < -55 DEGREES)
  // OTHERWISE, SET MAGNETIC GRID VARIATION TO 0.0
  *gv = 0.0;
  if (fabs(glat) >= 55.)
    {
      if (glat > 0.0 && glon >= 0.0) *gv = *dec-glon;
      if (glat > 0.0 && glon < 0.0) *gv = *dec+fabs(glon);
      if (glat < 0.0 && glon >= 0.0) *gv = *dec+glon;
      if (glat < 0.0 && glon < 0.0) *gv = *dec-fabs(glon);
      if (*gv > +180.0) *gv -= 360.0;
      if (*gv < -180.0) *gv += 360.0;
    }
    return time_ok;
}
