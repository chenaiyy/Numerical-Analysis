// SGFilter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#define TINY 1.0e-20
#define NR_END 1
#define FREE_ARG char*
static int iminarg1, iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1)<(iminarg2) ? \
	(iminarg1):(iminarg2))

float *vector (long nl, long nh);
int *ivector(long nl, long nh);
float **matrix(long nrl,long nrh,long ncl,long nch);
void free_vector(float *v,long nl, long nh);
void free_ivector(int *v,long nl, long nh);
void free_matrix(float **m,long nrl,long nrh,long ncl,long nch);
void lubksb(float **a, int n, int *indx, float b[]);
void ludcmp(float **a, int n, int *indx, float *d);
void savgol(float c[], int np, int nl, int nr, int ld, int m);


int _tmain(int argc, _TCHAR* argv[])
{
	int np = 5;
	int nl = 2;
	int nr = 2;
	int ld = 0;
	int m = 2;
	float *c;
	c = vector(1,np);
	savgol(c,np,nl,nr,ld,m);
	for(int i=1;i<=np;i++)
	printf("%f\t",c[i]);
	free_vector(c,1,np);
	return 0;
}

void savgol(float c[], int np, int nl, int nr, int ld, int m)
{
	int imj,ipj,j,k,kk,mm,*indx;
	float d,fac,sum,**a,*b;
	if (np < nl+nr+1 || nl < 0 || nr < 0 || ld > m || nl+nr < m)
		printf("bad args in savgol");
	indx=ivector(1,m+1);
	a=matrix(1,m+1,1,m+1);
	b=vector(1,m+1);
	for (ipj=0;ipj<=(m << 1);ipj++) { 
		sum=(ipj ? 0.0 : 1.0);
		for (k=1;k<=nr;k++) sum += pow((double)k,(double)ipj);
		for (k=1;k<=nl;k++) sum += pow((double)-k,(double)ipj);
		mm=IMIN(ipj,2*m-ipj);
		for (imj = -mm;imj<=mm;imj+=2) a[1+(ipj+imj)/2][1+(ipj-imj)/2]=sum;
	}
	ludcmp(a,m+1,indx,&d); 
	for (j=1;j<=m+1;j++) b[j]=0.0;
	b[ld+1]=1.0;
	lubksb(a,m+1,indx,b); 
	for (kk=1;kk<=np;kk++) c[kk]=0.0; 
	kk = 0;
	for (k = -nl;k<=nr;k++) {
		sum=b[1]; 
		fac=1.0;
		for (mm=1;mm<=m;mm++) sum += b[mm+1]*(fac *= k);
//		kk=((np-k) % np)+1;
		c[++kk]=sum;
//		c[kk]=sum;
//		printf("%f\t",c[kk]);
	}
	free_vector(b,1,m+1);
	free_matrix(a,1,m+1,1,m+1);
	free_ivector(indx,1,m+1);

}

void lubksb(float **a, int n, int *indx, float b[])
{
	int i,ii=0,ip,j;
	float sum;
	for (i=1;i<=n;i++){ 
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii)
			for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j];
		else if (sum) ii=i;
		b[i]=sum; 
	}
	for (i=n;i>=1;i--) {
		sum=b[i];
		for (j=i+1;j<=n;j++) sum -= a[i][j]*b[j];
		b[i]=sum/a[i][i]; 
	} 
}

void ludcmp(float **a, int n, int *indx, float *d)
{
	int i,imax,j,k;
	float big,dum,sum,temp;
	float *vv; 
	vv=vector(1,n);
	*d=1.0; 
	for (i=1;i<=n;i++) { 
		big=0.0; 
		for (j=1;j<=n;j++)
			if ((temp=fabs(a[i][j])) > big) big=temp;
		if (big == 0.0) 
			printf("Singular matrix in routine ludcmp");
		vv[i]=1.0/big; 
	}
	for (j=1;j<=n;j++) { 
		for (i=1;i<j;i++) { 
			sum=a[i][j];
			for (k=1;k<i;k++) sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
		}
		big=0.0; 
		for (i=j;i<=n;i++) { 
			sum=a[i][j]; 
			for (k=1;k<j;k++)
				sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
			if ( (dum=vv[i]*fabs(sum)) >= big) {
				big=dum;
				imax=i;
			}
		}
		if (j != imax) { 
			for (k=1;k<=n;k++) { 
				dum=a[imax][k];
				a[imax][k]=a[j][k];
				a[j][k]=dum;
			}
			*d = -(*d); 
			vv[imax]=vv[j]; 
		}
		indx[j]=imax;
		if (a[j][j] == 0.0) a[j][j]=TINY;

		if (j != n) {
			dum=1.0/(a[j][j]);
			for (i=j+1;i<=n;i++) a[i][j] *= dum;
		}
	} 
	free_vector(vv,1,n);
}

float *vector (long nl, long nh)
{
	float *v;
	v = (float*)malloc((size_t)((nh-nl+1+NR_END)*sizeof(float)));
	if(!v)
		printf("allocation failure");
	return v-nl+NR_END;
}
void free_vector(float *v,long nl, long nh)
{
	free((char*)v+nl-NR_END);
}

int *ivector(long nl, long nh)
{
	int *v;
	v = (int*)malloc((size_t)((nh-nl+1+NR_END)*sizeof(int))); 
	if(!v)
		printf("allocation failure");
	return v-nl+NR_END;
}

void free_ivector(int *v,long nl, long nh)
{
	free((FREE_ARG)v+nl-NR_END);
}

float **matrix(long nrl,long nrh,long ncl,long nch)
{
	long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
	float **m;
	/* allocate pointers to rows */
	m=(float **) malloc((size_t)((nrow+NR_END)*sizeof(float*)));
	if (!m) printf("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;
	/* allocate rows and set pointers to them */
	m[nrl]=(float *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
	if (!m[nrl]) printf("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
	/* return pointer to array of pointers to rows */
	return m;
}

void free_matrix(float **m,long nrl,long nrh,long ncl,long nch)
{
	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}
