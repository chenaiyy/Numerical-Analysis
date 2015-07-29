PRO Savgol_doc

  n = 401 ; number of points
  np = 4 ; number of peaks
  
  ; Form the baseline:
  y = REPLICATE(0.5, n)
  
  ; Sampling interval:
  dt = 0.1
  
  ; Index the array:
  x = dt*FINDGEN(n)
  
  ; Add each Gaussian peak:
  FOR i=0, np-1 DO BEGIN
    c = dt*(i + 0.5) * FLOAT(n)/np; Center of peak
    peak = 3 * (x-c) / (dt*(75. / 1.5 ^ i))
    ; Add Gaussian. Cutoff of -50 avoids underflow errors for
    ; tiny exponentials:
    y = y + EXP((-peak^2)>(-50))
  ENDFOR
  
  ; Add noise:
  y1 = y + 0.10 * RANDOMN(-121147, n)
  
  ; Display first plot
  Iplot, x, y1, NAME='Signal+Noise', VIEW_GRID=[1,2], $
  
    DIMENSIONS=[500,800]
    
  ; Get an object reference to the iTool and insert legend.
  void = Igetcurrent(TOOL=oTool)
  void = oTool->Doaction('Operations/Insert/Legend')
  
  Iplot, x, SMOOTH(y1, 33, /EDGE_TRUNCATE), /OVERPLOT, $
    COLOR=[255, 0, 0], $
    NAME='Smooth (width 33)'
  void = oTool->Doaction('Operations/Insert/LegendItem')
  
  ; Savitzky-Golay with 33, 4th degree polynomial:
  savgolFilter = Savgol(16, 16, 0, 4)
  Iplot, x, CONVOL(y1, savgolFilter, /EDGE_TRUNCATE), /OVERPLOT, $
    COLOR=[0, 0, 255], THICK=2, $
    NAME='Savitzky-Golay (width 33, 4th degree)'
  void = oTool->Doaction('Operations/Insert/LegendItem')
  
  Iplot, x, Deriv(x, Deriv(x, y)), YRANGE=[-4, 2], /VIEW_NEXT, $
    NAME='Second derivative'
    
  void = oTool->Doaction('Operations/Insert/Legend')
  
  order = 2
  ; Don't forget to normalize the coefficients.
  savgolFilter = Savgol(16, 16, order, 4)*(Factorial(order)/ $
    (dt^order))
  Iplot, x, CONVOL(y1, savgolFilter, /EDGE_TRUNCATE), /OVERPLOT, $
    COLOR=[0, 0, 255], THICK=2, $
    NAME='Savitzky-Golay(width 33, 4th degree, order 2)'
  void = oTool->Doaction('Operations/Insert/LegendItem')
  
  ; Reposition the legends
  Itranslate, 'legend*', X=-100, /DEVICE
  
END

