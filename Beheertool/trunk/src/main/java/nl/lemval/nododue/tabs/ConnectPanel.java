/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * ConnectPanel.java
 *
 * Created on 21-mrt-2010, 20:49:41
 */

package nl.lemval.nododue.tabs;

import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import java.awt.Cursor;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.TreeMap;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JOptionPane;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.NodoDueManagerView;
import nl.lemval.nododue.Options;
import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.util.listeners.OutputEventListener;
import nl.lemval.nododue.util.SerialCommunicator;
import org.jdesktop.application.Action;
import org.jdesktop.application.Application;
import org.jdesktop.application.ResourceMap;
import org.jdesktop.application.Task;

/**
 *
 * @author Michael
 */
public class ConnectPanel extends NodoBasePanel implements OutputEventListener {

    private DefaultComboBoxModel comPortModel;
    private HashMap<String, CommPortIdentifier> ports;
    private ResourceMap resourceMap;

    private static final String NEWLINE = System.getProperty("line.separator");

    /** Creates new form ConnectPanel */
    public ConnectPanel(NodoDueManagerView view) {
        super(view);

        resourceMap = Application.getInstance(NodoDueManager.class).getContext().getResourceMap(ConnectPanel.class);
        String start = resourceMap.getString("message.uitvoeren_scan");
        comPortModel = new DefaultComboBoxModel(new String[] {start});
        ports = new HashMap<String, CommPortIdentifier>();

        initComponents();

        portSelection.addItemListener(new ItemListener() {
            public void itemStateChanged(ItemEvent e) {
                if ( e.getStateChange() == e.SELECTED ) {
                    changeBaudrate((String)portSelection.getSelectedItem());
                }
            }
        });
        connectAction.setEnabled(false);

	// These buttons and labels are no longer needed.
        baudrateLabel.setVisible(false);
        databitsLabel.setVisible(false);
        stopbitsLabel.setVisible(false);
        parityLabel.setVisible(false);
        flowControlLabel.setVisible(false);
	baudSelection.setVisible(false);
        databitsSelection.setVisible(false);
        stopbitSelection.setVisible(false);
        paritySelection.setVisible(false);
        flowControl.setVisible(false);
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        scanAction = new javax.swing.JButton();
        databitsLabel = new javax.swing.JLabel();
        comPortLabel = new javax.swing.JLabel();
        databitsSelection = new javax.swing.JComboBox();
        baudrateLabel = new javax.swing.JLabel();
        stopbitsLabel = new javax.swing.JLabel();
        portSelection = new javax.swing.JComboBox();
        stopbitSelection = new javax.swing.JComboBox();
        baudSelection = new javax.swing.JComboBox();
        paritySelection = new javax.swing.JComboBox();
        parityLabel = new javax.swing.JLabel();
        connectAction = new javax.swing.JButton();
        flowControlLabel = new javax.swing.JLabel();
        jSeparator1 = new javax.swing.JSeparator();
        flowControl = new javax.swing.JComboBox();
        jLabel4 = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        serialOutput = new javax.swing.JTextArea();
        jButton1 = new javax.swing.JButton();

        setName("Form"); // NOI18N

        javax.swing.ActionMap actionMap = org.jdesktop.application.Application.getInstance(nl.lemval.nododue.NodoDueManager.class).getContext().getActionMap(ConnectPanel.class, this);
        scanAction.setAction(actionMap.get("scanPorts")); // NOI18N
        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(nl.lemval.nododue.NodoDueManager.class).getContext().getResourceMap(ConnectPanel.class);
        scanAction.setText(resourceMap.getString("scanAction.text")); // NOI18N
        scanAction.setName("scanAction"); // NOI18N

        databitsLabel.setText(resourceMap.getString("databitsLabel.text")); // NOI18N
        databitsLabel.setName("databitsLabel"); // NOI18N

        comPortLabel.setText(resourceMap.getString("comPortLabel.text")); // NOI18N
        comPortLabel.setName("comPortLabel"); // NOI18N

        databitsSelection.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "5", "6", "7", "8" }));
        databitsSelection.setSelectedIndex(3);
        databitsSelection.setEnabled(false);
        databitsSelection.setName("databitsSelection"); // NOI18N

        baudrateLabel.setText(resourceMap.getString("baudrateLabel.text")); // NOI18N
        baudrateLabel.setName("baudrateLabel"); // NOI18N

        stopbitsLabel.setText(resourceMap.getString("stopbitsLabel.text")); // NOI18N
        stopbitsLabel.setName("stopbitsLabel"); // NOI18N

        portSelection.setModel(comPortModel);
        portSelection.setName("portSelection"); // NOI18N

        stopbitSelection.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "1", "1.5", "2" }));
        stopbitSelection.setEnabled(false);
        stopbitSelection.setName("stopbitSelection"); // NOI18N

        baudSelection.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "4800", "9600", "19200", "38400", "57600" }));
        baudSelection.setSelectedIndex(2);
        baudSelection.setEnabled(false);
        baudSelection.setName("baudSelection"); // NOI18N

        paritySelection.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "EVEN", "MARK", "NONE", "ODD", "SPACE" }));
        paritySelection.setSelectedIndex(2);
        paritySelection.setEnabled(false);
        paritySelection.setName("paritySelection"); // NOI18N

        parityLabel.setText(resourceMap.getString("parityLabel.text")); // NOI18N
        parityLabel.setName("parityLabel"); // NOI18N

        connectAction.setAction(actionMap.get("connectSerial")); // NOI18N
        connectAction.setText(resourceMap.getString("connectAction.text")); // NOI18N
        connectAction.setName("connectAction"); // NOI18N

        flowControlLabel.setText(resourceMap.getString("flowControlLabel.text")); // NOI18N
        flowControlLabel.setName("flowControlLabel"); // NOI18N

        jSeparator1.setOrientation(javax.swing.SwingConstants.VERTICAL);
        jSeparator1.setName("jSeparator1"); // NOI18N

        flowControl.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "XONXOFF" }));
        flowControl.setEnabled(false);
        flowControl.setName("flowControl"); // NOI18N

        jLabel4.setFont(jLabel4.getFont().deriveFont(jLabel4.getFont().getStyle() | java.awt.Font.BOLD, jLabel4.getFont().getSize()+1));
        jLabel4.setText(resourceMap.getString("jLabel4.text")); // NOI18N
        jLabel4.setName("jLabel4"); // NOI18N

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        serialOutput.setColumns(20);
        serialOutput.setEditable(false);
        serialOutput.setRows(5);
        serialOutput.setName("serialOutput"); // NOI18N
        jScrollPane1.setViewportView(serialOutput);

        jButton1.setAction(actionMap.get("cleanOutput")); // NOI18N
        jButton1.setText(resourceMap.getString("jButton1.text")); // NOI18N
        jButton1.setName("jButton1"); // NOI18N

        org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(layout.createSequentialGroup()
                .addContainerGap()
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(comPortLabel)
                    .add(parityLabel)
                    .add(flowControlLabel)
                    .add(baudrateLabel)
                    .add(databitsLabel)
                    .add(stopbitsLabel))
                .add(18, 18, 18)
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING, false)
                    .add(connectAction, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .add(baudSelection, 0, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .add(scanAction, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 80, Short.MAX_VALUE)
                    .add(databitsSelection, 0, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .add(stopbitSelection, 0, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .add(paritySelection, 0, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .add(flowControl, 0, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .add(org.jdesktop.layout.GroupLayout.TRAILING, portSelection, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 99, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jSeparator1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 12, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(org.jdesktop.layout.GroupLayout.TRAILING, jScrollPane1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 348, Short.MAX_VALUE)
                    .add(org.jdesktop.layout.GroupLayout.TRAILING, jButton1)
                    .add(jLabel4))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(layout.createSequentialGroup()
                .addContainerGap()
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                    .add(org.jdesktop.layout.GroupLayout.LEADING, jSeparator1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 398, Short.MAX_VALUE)
                    .add(org.jdesktop.layout.GroupLayout.LEADING, layout.createSequentialGroup()
                        .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                            .add(layout.createSequentialGroup()
                                .add(jLabel4)
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(jScrollPane1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 342, Short.MAX_VALUE))
                            .add(layout.createSequentialGroup()
                                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                                    .add(portSelection, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                                    .add(comPortLabel))
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(scanAction)
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                                    .add(baudrateLabel)
                                    .add(baudSelection, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                                    .add(databitsLabel)
                                    .add(databitsSelection, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                                    .add(stopbitsLabel)
                                    .add(stopbitSelection, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                                    .add(parityLabel)
                                    .add(paritySelection, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                                    .add(flowControlLabel)
                                    .add(flowControl, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                                .add(18, 18, 18)
                                .add(connectAction)))
                        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                        .add(jButton1)))
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void changeBaudrate(String newValue) {
        if ( newValue.equals(Options.getInstance().getLastComportUsed())) {
            int size = baudSelection.getModel().getSize();
            long lubr = Options.getInstance().getLastBaudrateUsed();
            for (int i = 0; i < size; i++) {
                String elem = (String) baudSelection.getModel().getElementAt(i);
                if (Long.parseLong(elem) == lubr) {
                    baudSelection.setSelectedIndex(i);
                    break;
                }
            }
        }
    }

    @Action
    public Task scanPorts() {
        return new ScanPortsTask(org.jdesktop.application.Application.getInstance(nl.lemval.nododue.NodoDueManager.class));
    }

    private class ScanPortsTask extends org.jdesktop.application.Task<Object, Void> {
        private ResourceMap resourceMap;
        ScanPortsTask(org.jdesktop.application.Application app) {
            super(app);
            connectAction.setEnabled(false);
            getListener().readyForConnection(false);
            resourceMap = Application.getInstance(NodoDueManager.class).getContext().getResourceMap(ConnectPanel.class);
            setMessage(resourceMap.getString("message.start_scan"));

            comPortModel.removeAllElements();
            comPortModel.addElement(resourceMap.getString("message.wait"));
        }

        @Override protected Object doInBackground() {
            TreeMap<String, CommPortIdentifier> tree = new TreeMap<String, CommPortIdentifier>();
            Enumeration portList = CommPortIdentifier.getPortIdentifiers();
            while (portList.hasMoreElements()) {
                CommPortIdentifier portId = (CommPortIdentifier) portList.nextElement();
                if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
                    tree.put(portId.getName(), portId);
                }
            }
            return tree;
        }
        
        @Override protected void succeeded(Object result) {
            TreeMap<String, CommPortIdentifier> tree = (TreeMap<String, CommPortIdentifier>) result;
            ports.clear();
            ports.putAll(tree);
            connectAction.setEnabled(true);
            getListener().readyForConnection(true);

            StringBuilder sb = new StringBuilder();
            Options options = Options.getInstance();
            boolean hasPreviousPort = false;
            String previousPort = options.getLastComportUsed();

            comPortModel.removeAllElements();
            for (String portName : ports.keySet()) {
                comPortModel.addElement(portName);
                if ( portName.equals(previousPort) ) {
                    hasPreviousPort = true;
                }
                sb.append(portName);
                sb.append(", ");
            }
            int len = sb.length();
            if ( len > 2 )
                sb.delete(len-2, len);

            if ( hasPreviousPort ) {
                comPortModel.setSelectedItem(previousPort);
            }
            connectAction.setEnabled(true);
            setMessage(resourceMap.getString("message.end_scan", sb.toString()));
        }
    }

    @Action
    public void connectSerial() {
        setCursor(new Cursor(Cursor.WAIT_CURSOR));
        SerialCommunicator comm = NodoDueManager.getApplication().getSerialCommunicator();
        comm.setMessageListener(getListener());
        if (comm.isListening()) {
            comm.disconnect();
            getListener().disconnected();
            comm.removeOutputListener(this);
            scanAction.setEnabled(true);
        } else {
            String portname = (String) comPortModel.getSelectedItem();
            CommPortIdentifier portId = null;
            try {
                portId = CommPortIdentifier.getPortIdentifier(portname);
            } catch (NoSuchPortException e) {
                setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
                JOptionPane.showMessageDialog(this,
                        getResourceString("commport_disappeared.content", portname),
                        getResourceString("commport_disappeared.title"),
                        JOptionPane.ERROR_MESSAGE);
                return;
            }

            if (portId != null) {
                String baudRate = (String) baudSelection.getSelectedItem();
                Options options = Options.getInstance();

                comm.setPort(portId);
                comm.setBaud(baudRate);
                try {
                    scanAction.setEnabled(false);
                    handleClear();
                    comm.connect();
                    // Needed to settle the connection
                    try { Thread.sleep(2500); } catch (InterruptedException e) {}

                    String previousPort = options.getLastComportUsed();
                    long previousRate = options.getLastBaudrateUsed();
                    boolean force = !portname.equals(previousPort);
                    force = force || (previousRate != Long.parseLong(baudRate));
                    String message = initConnection(comm, force);
                    comm.addOutputListener(this);
                    handleOutputLine(message);
                    getListener().connected(message.length()>0?true:false);
                } catch (Exception e) {
                    scanAction.setEnabled(true);
                    getListener().showStatusMessage(e.getMessage());
                    handleOutputLine(resourceMap.getString("message.connect_failed"));
                    handleOutputLine(e.getMessage());
                }

                options.setLastBaudrateUsed(Integer.parseInt(baudRate));
                options.setLastComportUsed(portname);
            }
        }
        connectAction.setText(comm.isListening()
                ? resourceMap.getString("connectAction.end")
                : resourceMap.getString("connectAction.text"));
        setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
    }

    /**
     * Initializes the connection and checks for the unit and home code.
     * 
     * @param comm
     * @param force
     * @return
     */
    private String initConnection(SerialCommunicator comm, boolean force) {
        final StringBuilder result = new StringBuilder();
        Options options = Options.getInstance();
        if ( force || !options.hasScanned() ) {
            final String NL = System.getProperty("line.separator");
            OutputEventListener listener = new OutputEventListener() {
                public void handleOutputLine(String message) {
                    result.append(message);
                    result.append(NL);
                }
                public void handleClear() {}
            };
            comm.addOutputListener(listener);

            CommandInfo info = CommandLoader.get(CommandInfo.Name.Unit);
            comm.send(NodoCommand.getStatusCommand(info));

	    boolean timeout = false;
	    do {
                timeout = comm.waitCommand(200, 1000);
		options.scanLine(result.toString());
	    } while (!options.hasScanned() && !timeout);

            comm.removeOutputListener(listener);

	    if ( !options.hasScanned() ) {
		String portname = (String) comPortModel.getSelectedItem();
		JOptionPane.showMessageDialog(this,
                    getResourceString("comm.nodo_unrecognized.message", portname),
                    getResourceString("comm.nodo_unrecognized.title"),
                    JOptionPane.WARNING_MESSAGE);
	    } else {
		return getResourceString("comm.connected.ok", options.getNodoUnit(), options.getNodoHome());
	    }
        }
        return "";
    }

    public void handleOutputLine(String message) {
        serialOutput.append(message);
        serialOutput.append(NEWLINE);
        serialOutput.setCaretPosition(serialOutput.getText().length());
    }

    public void handleClear() {
        serialOutput.setText(null);
    }

    @Action
    public void cleanOutput() {
        handleClear();
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JComboBox baudSelection;
    private javax.swing.JLabel baudrateLabel;
    private javax.swing.JLabel comPortLabel;
    private javax.swing.JButton connectAction;
    private javax.swing.JLabel databitsLabel;
    private javax.swing.JComboBox databitsSelection;
    private javax.swing.JComboBox flowControl;
    private javax.swing.JLabel flowControlLabel;
    private javax.swing.JButton jButton1;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JLabel parityLabel;
    private javax.swing.JComboBox paritySelection;
    private javax.swing.JComboBox portSelection;
    private javax.swing.JButton scanAction;
    private javax.swing.JTextArea serialOutput;
    private javax.swing.JComboBox stopbitSelection;
    private javax.swing.JLabel stopbitsLabel;
    // End of variables declaration//GEN-END:variables

}
