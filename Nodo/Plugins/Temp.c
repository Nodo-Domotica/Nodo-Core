        if(TempActual<(TempSetpoint-DEVIATION_MAX)))
          {                                 
          Heating=HeatingTime;
          HeatingTime=0;
          }



                // Wacht op instelwaarde is bereikt => stop stoken      
      if(Heating>0 && TempActual>TempSetpoint)
        {
        Heating=0;
        // HeatingTime bevat aantal seconden dat is gestookt
        }

      if(TempActual>TempError)            // temperatuur ruimte neemt nog toe
        TempError=TempActual;             // Meet hoeveel graden teveel is doorgestookt
