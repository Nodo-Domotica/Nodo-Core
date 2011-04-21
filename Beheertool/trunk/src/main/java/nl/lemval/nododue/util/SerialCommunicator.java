/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.util;

import nl.lemval.nododue.cmd.CommandHistory;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.util.listeners.StatusMessageListener;
import nl.lemval.nododue.util.listeners.OutputEventListener;
import gnu.io.CommPortIdentifier;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import gnu.io.UnsupportedCommOperationException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashSet;
import java.util.TooManyListenersException;
import java.util.logging.Level;
import java.util.logging.Logger;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.cmd.NodoResponse;
import org.jdesktop.application.Application;
import org.jdesktop.application.ResourceMap;

/**
 * 
 * @author Michael
 */
public class SerialCommunicator implements Runnable, SerialPortEventListener {

    public static final int MIN_WAIT = 50;
    
    private int baudrate = 19200;
    private CommPortIdentifier port = null;
    private HashSet<OutputEventListener> outputListeners = new HashSet<OutputEventListener>();
    private StatusMessageListener statusListener = null;
    private InputStream inputStream;
    private OutputStream outputStream;
    private SerialPort serialPort;
    private Thread readThread;
    private StringBuilder sendBuffer = new StringBuilder();
    private static final Object sync = new Object();
    private static final Object data = new Object();
    private static final int XON = 17;
    private static final int XOFF = 19;
    private boolean sendingAllowed = true;
    private CommandHistory history = new CommandHistory();
    private StringBuilder previousContent = new StringBuilder();

    public boolean isListening() {
        return (serialPort != null);
    }

    public CommandHistory getHistory() {
        return history;
    }

    public void disconnect() {
        if (isListening()) {
            synchronized (data) {
                data.notifyAll();
            }

            serialPort.notifyOnDataAvailable(false);
            try {
                inputStream.close();
                outputStream.close();
            } catch (IOException ex) {
                Logger.getLogger(SerialCommunicator.class.getName()).log(
                        Level.SEVERE, null, ex);
            }
            inputStream = null;
            outputStream = null;
            serialPort.removeEventListener();
            serialPort.close();
            serialPort = null;

            for (OutputEventListener outputListener : outputListeners) {
                outputListener.handleClear();
            }

            ResourceMap resourceMap = Application.getInstance(NodoDueManager.class).getContext().getResourceMap(getClass());
            showMessage(resourceMap.getString("connect.closed"));
        }
    }

    public void setPort(CommPortIdentifier portId) {
        this.port = portId;
    }

    public void setBaud(String rate) {
        try {
            baudrate = Integer.parseInt(rate);
        } catch (Exception e) {
            if (statusListener != null) {
                statusListener.showStatusMessage("Error setting baudrate: "
                        + e.getLocalizedMessage());
            }
        }
    }

    public void addOutputListener(OutputEventListener listener) {
        outputListeners.add(listener);
    }

    public void removeOutputListener(OutputEventListener listener) {
        outputListeners.remove(listener);
    }

    public void setMessageListener(StatusMessageListener statusListener) {
        this.statusListener = statusListener;
    }

    private void showMessage(String message) {
        if (statusListener != null) {
            statusListener.showStatusMessage(message);
        }
    }

    public void connect() throws Exception {

        ResourceMap resourceMap = Application.getInstance(NodoDueManager.class).getContext().getResourceMap(getClass());

        showMessage(resourceMap.getString("connect.start"));
        try {
            serialPort = (SerialPort) port.open("NodoDue Manager", 5000);
        } catch (PortInUseException ex) {
            throw new Exception(resourceMap.getString("connect.port_in_use",
                    ex.getMessage()));
        }

        try {
            inputStream = serialPort.getInputStream();
        } catch (IOException ex) {
            throw new Exception(resourceMap.getString("connect.no_info",
                    ex.getMessage()));
        }
        try {
            serialPort.addEventListener(this);
        } catch (TooManyListenersException ex) {
            throw new Exception(resourceMap.getString("connect.no_slot",
                    ex.getMessage()));
        }
        serialPort.notifyOnDataAvailable(true);

        try {
            serialPort.setSerialPortParams(baudrate, SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
//			serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_XONXOFF_IN
//					| SerialPort.FLOWCONTROL_XONXOFF_OUT);
            serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
        } catch (UnsupportedCommOperationException ex) {
            throw new Exception(resourceMap.getString("connect.invalid_config",
                    ex.getMessage()));
        }
        for (OutputEventListener outputListener : outputListeners) {
            outputListener.handleClear();
        }
        readThread = new Thread(this);
        readThread.start();

        showMessage(resourceMap.getString("connect.done"));
    }

    public void run() {
        try {
            outputStream = serialPort.getOutputStream();
        } catch (IOException ex) {
            Logger.getLogger(SerialCommunicator.class.getName()).log(
                    Level.SEVERE, null, ex);
        }

        String osname = System.getProperty("os.name", "").toLowerCase();
        if (osname.startsWith("linux") == false) {
            serialPort.notifyOnOutputEmpty(true);
        }

        while (outputStream != null) {
            if (sendingAllowed && sendBuffer.length() > 0) {
                String command = null;
                synchronized (sync) {
                    command = sendBuffer.toString();
                    sendBuffer.delete(0, sendBuffer.length());
                }
                try {
                    String[] elems = command.split(";");
                    for (int i = 0; i < elems.length; i++) {
                        history.addRequest(elems[i]);
//                        System.out.println("Sending : '"+elems[i]+";'");
                        while ( !sendingAllowed ) {
                            Thread.sleep(50);
                        }
                        outputStream.write((elems[i] + ";").getBytes());
                        outputStream.flush();
                    }
                } catch (Exception ex) {
                    Logger.getLogger(SerialCommunicator.class.getName()).log(
                            Level.SEVERE, null, ex);
                    disconnect();
                }
            }
            // Wait half a second before retrying to send commands again
            // This gives the program a way relax
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
            }
        }
    }

    public boolean send(NodoCommand command) {
        return sendRaw(command.getRawCommand());
    }

    public boolean sendRaw(String message) {
        // TODO: i18n
        if (isListening() && message != null) {
            // busy = true;
            showMessage(message + " "
                    + NodoDueManager.getMessage("Connection.connected"));
            synchronized (sync) {
                sendBuffer.append(message);
            }
            return true;
        }
        if (message == null) {
            showMessage(NodoDueManager.getMessage("Connection.command_is_local"));
        } else {
            showMessage(message + " "
                    + NodoDueManager.getMessage("Connection.disconnected"));
        }
        return false;
    }

    public void serialEvent(SerialPortEvent event) {
        switch (event.getEventType()) {
            case SerialPortEvent.BI: /* Break interrupt */
            case SerialPortEvent.OE: /* Overrun error */
            case SerialPortEvent.FE: /* Framing error */
            case SerialPortEvent.PE: /* Parity error */
            case SerialPortEvent.CD: /* Carrier detect */
            case SerialPortEvent.CTS:/* Clear to send */
            case SerialPortEvent.DSR:/* Data set ready */
            case SerialPortEvent.RI: /* Ring indicator */
            case SerialPortEvent.OUTPUT_BUFFER_EMPTY: /* 2 */
                System.out.println("Got " + event.getEventType());
                break;
            case SerialPortEvent.DATA_AVAILABLE:
                handleData();
                break;
        }
    }

    private void handleData() {
        // we get here if data has been received
        byte[] readBuffer = new byte[1024];
        try {
            int numBytes = 0;
            while (inputStream.available() > 0) {
                numBytes += inputStream.read(readBuffer, numBytes, inputStream.available());
            }

            String read = new String(readBuffer, 0, numBytes);
            int start = 0;
            for (int i = 0; i < read.length(); i++) {
                char c = read.charAt(i);
                if (c == XON) {
                    sendingAllowed = true;
                    start++;
                    if ( start == read.length() ) {
                        return;
                    }
//                    System.out.println("Got XON");
//                    System.out.println(read.substring(start, i) + "*" + read.substring(i+1));
                } else if (c == XOFF) {
                    sendingAllowed = false;
                    start++;
//                    System.out.println("Got XOFF");
                    if ( start == read.length() ) {
                        return;
                    }
//                    System.out.println(read.substring(start, i) + "#" + read.substring(i+1));
                } else if (c == '\r' || c == '\n') {
//                    System.out.println("Got NL");
                    final String readData = read.substring(start, i);
                    previousContent.append(readData);
                    String data = previousContent.toString();
                    previousContent.delete(0, previousContent.length());
                    if (data.length() > 0) {
                        System.out.println("Received: '"+data+"'");
                        history.addResponse(data);
                        for (OutputEventListener outputListener : outputListeners) {
                            outputListener.handleOutputLine(data);
                        }
                        NodoResponse[] responses = NodoResponse.getResponses(data);
                        for (OutputEventListener outputListener : outputListeners) {
                            outputListener.handleNodoResponses(responses);
                        }
                    }
                    start = i + 1;
                }
            }
            if (start == read.length()) {
                // Nothing more on data to expect
                synchronized (data) {
                    data.notifyAll();
                }
            } else {
                int end = read.length()-1;
                while ( end > 0 && read.charAt(end) == 0 ) {
                    end--;
                }
                final String readData = read.substring(start, end+1);
                // Keep data for the next event
                previousContent.append(readData);
            }
        } catch (IOException e) {
            Logger.getLogger(SerialCommunicator.class.getName()).log(
                    Level.SEVERE, null, e);
        }
    }

    public boolean waitCommand() {
        return waitCommand(MIN_WAIT, MIN_WAIT);
    }

    public boolean waitCommand(int minWait) {
        return waitCommand(minWait, minWait);
    }

    public boolean waitCommand(int minWait, int maxWait) {
        if (minWait > maxWait) {
            System.out.println("Error -> Wrong order");
            int tmp = maxWait;
            maxWait = minWait;
            minWait = tmp;
        }
        try {
            long start = System.currentTimeMillis();
            long passedTime = 0;
            while (passedTime < minWait) {
                synchronized (data) {
                    data.wait(maxWait - passedTime);
                }
                passedTime = System.currentTimeMillis() - start;
            }
            return (passedTime >= maxWait);
        } catch (InterruptedException e1) {
            e1.printStackTrace();
        }
        return false;
    }
}
