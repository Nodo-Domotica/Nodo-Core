/**************************************************************************\

    This file is part of Nodo Due, © Copyright Kenneth Rovers

    Nodo Due is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nodo Due is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Nodo Due.  If not, see <http://www.gnu.org/licenses/>.

\**************************************************************************/


/*

 Hier bevinden zich de de routines voor line code
 * PWM       Pulse width modulation / pulse coded - Encoding in de lengte van het high signaal
 * PDM       Pulse distance modulation / space coded - Encoding in de lengte van het low signaal
 * PWDM      Pulse width distance modulation - Encoding in beide (bipolar) met constante totale pulse breedte

*/

void encodePWM(byte i, unsigned int T, byte nP, byte nS, byte nL, boolean y) { // pulse width modulation / pulse coded
  byte n;
  if (y==0) { n = nS; } else { n = nL; }
  encodePulse(2*i, T, n); encodePulse(2*i+1, T, nP);
}

void encodePDM(byte i, unsigned int T, byte nP, byte nS, byte nL, boolean y) { // pulse distance modulation / space coded
  byte n;
  if (y==0) { n = nS; } else { n = nL; }
  encodePulse(2*i, T, nP); encodePulse(2*i+1, T, n);
}

void encodePWDM(byte i, unsigned int T, byte nP, byte nS, byte nL, boolean y) { // pulse width distance modulation
  byte n0, n1;
  if (y==0) { n0 = nS; n1 = nL; } else { n0 = nL; n1 = nS; }
  encodePulse(2*i, T, n0); encodePulse((2*i)+1, T, n1);
}

byte decodePWM(byte i, unsigned int T, byte nP, byte nS, byte nL) { // pulse width modulation
  byte y = (decodePulse(2*i, T, nS, nL) << 1 | decodePulse(2*i+1, T, nP, 0));
  if (y==0) { return 0; }
  if (y==2) { return 1; }
  return 0xFF;
}

byte decodePDM(byte i, unsigned int T, byte nP, byte nS, byte nL) { // pulse distance modulation
  byte y = (decodePulse(2*i, T, nP, 0) << 1 | decodePulse(2*i+1, T, nS, nL));
  if (y==0) { return 0; }
  if (y==1) { return 1; }
  return 0xFF;
}

byte decodePWDM(byte i, unsigned int T, byte nP, byte nS, byte nL) { // pulse width distance modulation
  byte y = (decodePulse(2*i, T, nS, nL) << 1 | decodePulse(2*i+1, T, nS, nL));
  if (y==0) { return 2; } // short 0
  if (y==1) { return 0; }
  if (y==2) { return 1; }
  return 0xFF;
}

void encodePulse(byte i, unsigned int T, byte n) {
  RawSignal[i+1] = n*T;
}

byte decodePulse(byte i, unsigned int T, byte n0, byte n1) {
  unsigned int t; //, t0, t1;
  t = RawSignal[i+1];
  if ((t > ((n0-1)*T)) && (t < ((n0+1)*T))) { return 0; }
  if ((t > ((n1-1)*T)) && (t < ((n1+1)*T))) { return 1; }
  return 0xFF;
}
