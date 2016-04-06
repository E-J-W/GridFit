//generate data to be plotted, by selecting datapoints nearest to the fit vertex
void getPlotDataNearMin(const data * d, const parameters * p, const fit_results * fr, plot_data * pd)
{
  int i,j,k;
  long double minDist,mdv;
  for(i=0;i<p->numVar;i++)
    {
      minDist=BIG_NUMBER;
      for(j=0;j<d->lines;j++)
        {
          mdv=(d->x[i][j]) - (fr->fitVert[i]);
          if(mdv<0) mdv=mdv*-1.0; //for some reason abs() doesn't work with long double?
          if(mdv < minDist)
            {
              minDist=mdv;
              pd->fixedParVal[i]=d->x[i][j];
            }
        }
    }
    
  
  //generate plot data
  int useDataPoint=0;
  if(strcmp(p->plotMode,"1d")==0)
    {
      memset(pd->plotDataSize,0,sizeof(pd->plotDataSize));
      for(i=0;i<p->numVar;i++)//plot index (x,y,z)
        for(j=0;j<d->lines;j++)
          {
            //check whether the other (non plot index) variables are all at their fixed values
            useDataPoint=1;
            for(k=0;k<p->numVar;k++)//parameter index (x,y,z)
              if(k!=i)//is variable fixed?
                if(d->x[k][j]!=pd->fixedParVal[k])
                  useDataPoint=0;
             
            if(useDataPoint==1)
              {
                for(k=0;k<=p->numVar;k++)//parameter index (x,y,z)
                  pd->data[i][k][pd->plotDataSize[i]]=((double)d->x[k][j]);
                pd->plotDataSize[i]++;
              }
          }
    }
  else if((p->numVar==3)&&(strcmp(p->plotMode,"2d")==0))
    {
      memset(pd->plotDataSize,0,sizeof(pd->plotDataSize));
      for(i=0;i<3;i++)//plot index (yz,xz,xy)
        for(j=0;j<d->lines;j++)
          if(d->x[i][j]==pd->fixedParVal[i])
            {
              for(k=0;k<=p->numVar;k++)//parameter index (x,y,z,value)
                pd->data[i][k][pd->plotDataSize[i]]=((double)d->x[k][j]);
              pd->plotDataSize[i]++;
            }
    }
  else if((p->numVar==2)&&(strcmp(p->plotMode,"2d")==0))
    {
      memset(pd->plotDataSize,0,sizeof(pd->plotDataSize));
      for(i=0;i<d->lines;i++)
        {
          for(j=0;j<=p->numVar;j++)//parameter index (x,y,z,value)
            pd->data[0][j][pd->plotDataSize[0]]=((double)d->x[j][i]);
          pd->plotDataSize[0]++;
        }
    }
  else if (p->plotData!=0)
    {
      printf("ERROR: Plotting mode not recognized!\nPlot mode: %s\n",p->plotMode);
    }

}

void plotData(const data * d, const parameters * p, const fit_results * fr, plot_data * pd)
{
  int i;
  char str[256];
  plotOpen=1; 
  handle=gnuplot_init();
  printf("\nDATA PLOTS\n----------\nUse 'l' in the plotting window to switch between linear and logarithmic scale.\n");
  
  if(strcmp(p->plotMode,"1d")==0)
    {
      for(i=0;i<p->numVar;i++)
        {
          gnuplot_setstyle(handle,"points"); //set style for grid points
          gnuplot_cmd(handle,"set ylabel 'Value'");
          sprintf(str,"set xlabel 'Parameter %i'",i+1);
          gnuplot_cmd(handle,str);
          gnuplot_plot_xy(handle, pd->data[i][i], pd->data[i][p->numVar], pd->plotDataSize[i], "Data");
          gnuplot_setstyle(handle,"lines");//set style for fit data
          //generate fit data functional forms
          if(p->numVar==3)
            {
              if(i==0)
                sprintf(str, "%Lf*(x**2) + %Lf*(%Lf**2) + %Lf*(%Lf**2) + %Lf*x*%Lf + %Lf*x*%Lf + %Lf*%Lf*%Lf + %Lf*x + %Lf*%Lf + %Lf*%Lf + %Lf",fr->a[0],fr->a[1],pd->fixedParVal[1],fr->a[2],pd->fixedParVal[2],fr->a[3],pd->fixedParVal[1],fr->a[4],pd->fixedParVal[2],fr->a[5],pd->fixedParVal[1],pd->fixedParVal[2],fr->a[6],fr->a[7],pd->fixedParVal[1],fr->a[8],pd->fixedParVal[2],fr->a[9]);
              else if(i==1)
                sprintf(str, "%Lf*(%Lf**2) + %Lf*(x**2) + %Lf*(%Lf**2) + %Lf*x*%Lf + %Lf*%Lf*%Lf + %Lf*x*%Lf + %Lf*x + %Lf*%Lf + %Lf*%Lf + %Lf",fr->a[0],pd->fixedParVal[0],fr->a[1],fr->a[2],pd->fixedParVal[2],fr->a[3],pd->fixedParVal[0],fr->a[4],pd->fixedParVal[0],pd->fixedParVal[2],fr->a[5],pd->fixedParVal[2],fr->a[7],fr->a[6],pd->fixedParVal[0],fr->a[8],pd->fixedParVal[2],fr->a[9]);
              else if(i==2)
                sprintf(str, "%Lf*(%Lf**2) + %Lf*(%Lf**2) + %Lf*(x**2) + %Lf*%Lf*%Lf + %Lf*x*%Lf + %Lf*x*%Lf + %Lf*x + %Lf*%Lf + %Lf*%Lf + %Lf",fr->a[0],pd->fixedParVal[0],fr->a[1],pd->fixedParVal[1],fr->a[2],fr->a[3],pd->fixedParVal[0],pd->fixedParVal[1],fr->a[4],pd->fixedParVal[0],fr->a[5],pd->fixedParVal[1],fr->a[8],fr->a[6],pd->fixedParVal[0],fr->a[7],pd->fixedParVal[1],fr->a[9]);
            }
          else if(p->numVar==2)
            {
              if(i==0)
                sprintf(str, "%Lf*(x**2) + %Lf*(%Lf**2) + %Lf*x*%Lf + %Lf*x + %Lf*%Lf + %Lf",fr->a[0],fr->a[1],pd->fixedParVal[1],fr->a[2],pd->fixedParVal[1],fr->a[3],fr->a[4],pd->fixedParVal[1],fr->a[5]);
              else if(i==1)
                sprintf(str, "%Lf*(x**2) + %Lf*(%Lf**2) + %Lf*x*%Lf + %Lf*x + %Lf*%Lf + %Lf",fr->a[1],fr->a[0],pd->fixedParVal[0],fr->a[2],pd->fixedParVal[0],fr->a[4],fr->a[3],pd->fixedParVal[0],fr->a[5]);
            }
          gnuplot_plot_equation(handle, str, "Fit");       
          printf("Showing plot for parameter %i.\n",i+1);
          if(p->numVar==3)
            {
              if(i==0)
                printf("Parameter %i fixed to %Lf\nParameter %i fixed to %Lf\n",2,pd->fixedParVal[1],3,pd->fixedParVal[2]);
              if(i==1)
                printf("Parameter %i fixed to %Lf\nParameter %i fixed to %Lf\n",1,pd->fixedParVal[0],3,pd->fixedParVal[2]);
              if(i==2)
                printf("Parameter %i fixed to %Lf\nParameter %i fixed to %Lf\n",1,pd->fixedParVal[0],2,pd->fixedParVal[1]);
            }
          else if(p->numVar==2)
            {
              if(i==0)
                printf("Parameter %i fixed to %Lf\n",2,pd->fixedParVal[1]);
              if(i==1)
                printf("Parameter %i fixed to %Lf\n",1,pd->fixedParVal[0]);
            }
          printf("%i data points available for plot.\n",pd->plotDataSize[i]);
          printf("Press [ENTER] to continue.");
          getc(stdin);
          gnuplot_resetplot(handle);
        }
    }
  else if(strcmp(p->plotMode,"2d")==0)
    {
      if(p->numVar==3)
        {
          for(i=0;i<p->numVar;i++)
            {
              gnuplot_setstyle(handle,"points"); //set style for grid points
              if(i==0)
                {
                  gnuplot_plot_xyz(handle, pd->data[i][1], pd->data[i][2], pd->data[i][p->numVar], pd->plotDataSize[i], "Data");
                  sprintf(str,"set xlabel 'Parameter 2'; set ylabel 'Parameter 3';");
                }
              if(i==1)
                {
                  gnuplot_plot_xyz(handle, pd->data[i][0], pd->data[i][2], pd->data[i][p->numVar], pd->plotDataSize[i], "Data");
                  sprintf(str,"set xlabel 'Parameter 1'; set ylabel 'Parameter 3';");
                }
              if(i==2)
                {
                  gnuplot_plot_xyz(handle, pd->data[i][0], pd->data[i][1], pd->data[i][p->numVar], pd->plotDataSize[i], "Data");
                  sprintf(str,"set xlabel 'Parameter 1'; set ylabel 'Parameter 2';");
                }
              gnuplot_cmd(handle,str);
              gnuplot_setstyle(handle,"lines");
              gnuplot_cmd(handle,"set grid");//set style for fit data
              //generate fit data functional forms
              if(i==0)//x=par[1],y=par[2]
                sprintf(str, "%Lf*(%Lf**2) + %Lf*(x**2) + %Lf*(y**2) + %Lf*%Lf*x + %Lf*%Lf*y + %Lf*x*y + %Lf*%Lf + %Lf*x + %Lf*y + %Lf",fr->a[0],pd->fixedParVal[0],fr->a[1],fr->a[2],fr->a[3],pd->fixedParVal[0],fr->a[4],pd->fixedParVal[0],fr->a[5],fr->a[6],pd->fixedParVal[0],fr->a[7],fr->a[8],fr->a[9]);
              else if(i==1)//x=par[0],y=par[2]
                sprintf(str, "%Lf*(x**2) + %Lf*(%Lf**2) + %Lf*(y**2) + %Lf*x*%Lf + %Lf*x*y + %Lf*%Lf*y + %Lf*x + %Lf*%Lf + %Lf*y + %Lf",fr->a[0],fr->a[1],pd->fixedParVal[1],fr->a[2],fr->a[3],pd->fixedParVal[1],fr->a[4],fr->a[5],pd->fixedParVal[1],fr->a[6],fr->a[7],pd->fixedParVal[1],fr->a[8],fr->a[9]);
              else if(i==2)//x=par[0],y=par[1]
                sprintf(str, "%Lf*(x**2) + %Lf*(y**2) + %Lf*(%Lf**2) + %Lf*x*y + %Lf*x*%Lf + %Lf*y*%Lf + %Lf*x + %Lf*y + %Lf*%Lf + %Lf",fr->a[0],fr->a[1],fr->a[2],pd->fixedParVal[2],fr->a[3],fr->a[4],pd->fixedParVal[2],fr->a[5],pd->fixedParVal[2],fr->a[6],fr->a[7],fr->a[8],pd->fixedParVal[2],fr->a[9]);
              gnuplot_plot_equation(handle, str, "Fit");
              printf("Showing surface plot with parameter %i fixed to %Lf\n",i+1,pd->fixedParVal[i]);
              printf("%i data points available for plot.\n",pd->plotDataSize[i]);
              printf("Press [ENTER] to continue.");
              getc(stdin);
              gnuplot_resetplot(handle);
            }
        }
      else if(p->numVar==2)
        {
          gnuplot_setstyle(handle,"points"); //set style for grid points
          gnuplot_plot_xyz(handle, pd->data[0][0], pd->data[0][1], pd->data[0][p->numVar], pd->plotDataSize[0], "Data");
          sprintf(str,"set xlabel 'Parameter 1'; set ylabel 'Parameter 2';");
          gnuplot_cmd(handle,str);
          gnuplot_setstyle(handle,"lines");
          gnuplot_cmd(handle,"set grid");//set style for fit data
          //generate fit data functional forms
          sprintf(str, "%Lf*(x**2) + %Lf*(y**2) + %Lf*x*y + %Lf*x + %Lf*y + %Lf",fr->a[0],fr->a[1],fr->a[2],fr->a[3],fr->a[4],fr->a[5]);
          gnuplot_plot_equation(handle, str, "Fit");
          printf("Showing surface plot.\n");
          printf("%i data points available for plot.\n",pd->plotDataSize[0]);
          printf("Press [ENTER] to continue.");
          getc(stdin);
          gnuplot_resetplot(handle);
        }
    }
  else
    {
      printf("ERROR: The plot mode '%s' defined in the data file '%s' is not supported!\n",p->plotMode,p->filename);
      exit(-1);
    }

}

//function run after CTRL-C, used to clean up temporary files generated
//by the plotting library
void sigint_cleanup()
{
  if(plotOpen==1)
    gnuplot_close(handle); //cleans up temporary files  
  exit(1); 
}
