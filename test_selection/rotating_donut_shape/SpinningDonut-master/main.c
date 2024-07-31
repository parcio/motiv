                      #include <stdio.h>
                 #include <math.h>//OOOOOOOOO
             #define at(x, y)(x + y * (W + 1))//O
         int main(){int W=60,H=30,R;double r1=0.5,r2
       =1,k2=4.2,k1=W*k2*3/(8*(r1+r2)),PI=3.1415,rx=0//
     ,rz=0,i=0,j=0,zb[H*W+H],cx,cy,x,y,z;char b[H*W+H];//
   for(;;R++){printf("\x1b[0;0H");for(int i=0;i<W*H+H;++i){
  b[i]=' ';zb[i]=0;}for(int        i=0;i<H;++i){b[i*(W+1)+W]
 ='\n';}for(i=0;i<2*PI;                i+=PI/100){for(j=0;j<2
 *PI;j+=PI/100){cx=r2                    +r1*cos(i);cy=r1*sin
(i);z=k2+cos(rx)*cx*                      sin(j)+cy*sin(rx);x=
(cx*(cos(rz)*cos(j)+                      sin(rx)*sin(rz)*sin(j
))-cy*cos(rx)*sin(rz                      ))/(z*3);y=(cx*(sin//
(rz)*cos(j)-sin(rx)*                      cos(rz)*sin(j))+cy*//
 cos(rx)*cos(rz))/(z*                    3);z=1/((z-1)/(z*3));
 int xp=(W/2+k1)*z*x/2+                W/2;int yp=(H/2-k1)*z*y
  /2+H/2;double lum=cos(j)*        cos(i)*sin(rz)-cos(rx)*cos
   (i)*sin(j)-sin(rx)* sin(i)+cos(rz)*(cos(rx)*sin(i)-cos(i)
     *sin(rx)*sin(j));if(lum>0){if(xp<W&&xp>=0&&yp<H&&yp>=
       0&&z>zb[at(xp,yp)]){zb[at(xp,yp)]=z;b[at(xp,yp)]=
         ".,-:=*+*#%@"[(int)(lum*7.3)];}}}}fwrite(b,//
              sizeof(b),1,stdout);rx+=0.006;rz+=//
                   0.003;if(R>30)return 0;}}
