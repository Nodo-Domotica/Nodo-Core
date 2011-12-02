/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * NodoCommandPanel.java
 *
 * Created on 21-mrt-2010, 19:10:28
 */
package nl.lemval.nododue.tabs;

import java.awt.GridLayout;
import java.awt.event.KeyEvent;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.NodoDueManagerView;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.CommandType;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.component.EventActionPanel;
import nl.lemval.nododue.util.listeners.OutputEventListener;
import nl.lemval.nododue.util.SerialCommunicator;
import org.apache.commons.lang.StringUtils;
import org.jdesktop.application.Action;

/**
 *
 * @author Michael
 */
public class NodoCommandPanel extends NodoBasePanel {

    private EventActionPanel actionPanel;
    public static final String NEWLINE = System.getProperty("line.separator");

    /** Creates new form NodoCommandPanel */
    public NodoCommandPanel(NodoDueManagerView view) {
        super(view);

        // Initializes the panel with the actions only.
        actionPanel = new EventActionPanel(view, getResourceString("panel.name"));
        actionPanel.addAll(CommandLoader.getActions(CommandType.COMMAND));

        initComponents();

        commandPanel.setLayout(new GridLayout());
        commandPanel.add(actionPanel);
    }

    @Override
    public void setVisible(boolean aFlag) {
        super.setVisible(aFlag);
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        commandPanel = new javax.swing.JPanel();
        commandOutputLabel = new javax.swing.JLabel();
        jScrollPane2 = new javax.swing.JScrollPane();
        commandOutput = new javax.swing.JTextArea();
        rawInput = new javax.swing.JTextField();
        commandOutputLabel1 = new javax.swing.JLabel();
        commandRun = new javax.swing.JButton();

        setName("Form"); // NOI18N

        commandPanel.setFocusable(false);
        commandPanel.setName("commandPanel"); // NOI18N

        org.jdesktop.layout.GroupLayout commandPanelLayout = new org.jdesktop.layout.GroupLayout(commandPanel);
        commandPanel.setLayout(commandPanelLayout);
        commandPanelLayout.setHorizontalGroup(
            commandPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(0, 217, Short.MAX_VALUE)
        );
        commandPanelLayout.setVerticalGroup(
            commandPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(0, 330, Short.MAX_VALUE)
        );

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(nl.lemval.nododue.NodoDueManager.class).getContext().getResourceMap(NodoCommandPanel.class);
        commandOutputLabel.setText(resourceMap.getString("commandOutputLabel.text")); // NOI18N
        commandOutputLabel.setName("commandOutputLabel"); // NOI18N

        jScrollPane2.setFocusable(false);
        jScrollPane2.setName("jScrollPane2"); // NOI18N

        commandOutput.setColumns(20);
        commandOutput.setEditable(false);
        commandOutput.setRows(5);
        commandOutput.setFocusable(false);
        commandOutput.setName("commandOutput"); // NOI18N
        jScrollPane2.setViewportView(commandOutput);

        rawInput.setText(resourceMap.getString("rawInput.text")); // NOI18N
        rawInput.setName("rawInput"); // NOI18N
        rawInput.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                rawInputCheck(evt);
            }
        });

        commandOutputLabel1.setFont(commandOutputLabel1.getFont().deriveFont(commandOutputLabel1.getFont().getStyle() | java.awt.Font.BOLD, commandOutputLabel1.getFont().getSize()+1));
        commandOutputLabel1.setText(resourceMap.getString("commandOutputLabel1.text")); // NOI18N
        commandOutputLabel1.setName("commandOutputLabel1"); // NOI18N

        javax.swing.ActionMap actionMap = org.jdesktop.application.Application.getInstance(nl.lemval.nododue.NodoDueManager.class).getContext().getActionMap(NodoCommandPanel.class, this);
        commandRun.setAction(actionMap.get("executeNodoCommand")); // NOI18N
        commandRun.setText(resourceMap.getString("commandRun.text")); // NOI18N
        commandRun.setToolTipText(resourceMap.getString("commandRun.toolTipText")); // NOI18N
        commandRun.setActionCommand(resourceMap.getString("commandRun.actionCommand")); // NOI18N
        commandRun.setName("commandRun"); // NOI18N

        org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(layout.createSequentialGroup()
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                    .add(commandPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                    .add(commandRun, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 94, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(commandOutputLabel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 224, Short.MAX_VALUE)
                    .add(org.jdesktop.layout.GroupLayout.TRAILING, commandOutputLabel1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 224, Short.MAX_VALUE)
                    .add(org.jdesktop.layout.GroupLayout.TRAILING, jScrollPane2, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 224, Short.MAX_VALUE)
                    .add(rawInput, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 224, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(org.jdesktop.layout.GroupLayout.TRAILING, layout.createSequentialGroup()
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                    .add(layout.createSequentialGroup()
                        .addContainerGap()
                        .add(commandOutputLabel1)
                        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                        .add(jScrollPane2, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 293, Short.MAX_VALUE))
                    .add(commandPanel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(commandOutputLabel)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                    .add(rawInput, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                    .add(commandRun))
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void rawInputCheck(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_rawInputCheck
        if (evt.getKeyCode() == KeyEvent.VK_ENTER) {
            sendCommand(rawInput.getText());
            evt.consume();
            rawInput.setText(null);
        }
    }//GEN-LAST:event_rawInputCheck

    public void sendCommand(String message) {
        SerialCommunicator comm =
                NodoDueManager.getApplication().getSerialCommunicator();
        comm.setMessageListener(getListener());
        commandOutput.setText(null);
        OutputEventListener lix = null;
        comm.addOutputListener(lix = new OutputEventListener() {

            public void handleOutputLine(String message) {
                commandOutput.append(message);
                commandOutput.append(NEWLINE);
            }

            public void handleClear() {
                commandOutput.setText(null);
            }
        });
        comm.sendRaw(message);
        comm.waitCommand(500);
        comm.removeOutputListener(lix);
    }

    @Action
    public void executeNodoCommand() {
        if ( StringUtils.isNotBlank(rawInput.getText())) {
            sendCommand(rawInput.getText());
            rawInput.setText(null);
            return;
        }

        NodoCommand cmd = actionPanel.getSelectedItem();
        if (cmd != null) {
            SerialCommunicator comm =
                    NodoDueManager.getApplication().getSerialCommunicator();
            comm.setMessageListener(getListener());
            commandOutput.setText(null);
            OutputEventListener lix = null;
            comm.addOutputListener(lix = new OutputEventListener() {

                public void handleOutputLine(String message) {
                    commandOutput.append(message);
                    commandOutput.append(NEWLINE);
                }

                public void handleClear() {
                    commandOutput.setText(null);
                }
            });
            comm.send(cmd);
            comm.waitCommand(500);
            comm.removeOutputListener(lix);
        }
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextArea commandOutput;
    private javax.swing.JLabel commandOutputLabel;
    private javax.swing.JLabel commandOutputLabel1;
    private javax.swing.JPanel commandPanel;
    private javax.swing.JButton commandRun;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JTextField rawInput;
    // End of variables declaration//GEN-END:variables
}
