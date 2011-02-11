/*
 * NodoDueManagerView.java
 */
package nl.lemval.nododue;

import nl.lemval.nododue.dialog.NodoDueManagerOptionBox;
import nl.lemval.nododue.dialog.NodoDueManagerAboutBox;
import nl.lemval.nododue.util.listeners.StatusMessageListener;
import org.jdesktop.application.Action;
import org.jdesktop.application.ResourceMap;
import org.jdesktop.application.SingleFrameApplication;
import org.jdesktop.application.FrameView;
import org.jdesktop.application.TaskMonitor;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.io.File;
import java.util.Arrays;
import java.util.Collection;
import javax.swing.Timer;
import javax.swing.Icon;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.filechooser.FileFilter;
import nl.lemval.nododue.dialog.CommandHistoryBox;
import nl.lemval.nododue.dialog.ConfigDateBox;
import nl.lemval.nododue.dialog.DevicesBox;
import nl.lemval.nododue.dialog.LearnCommandBox;
import nl.lemval.nododue.dialog.UnitSelectorBox;
import nl.lemval.nododue.tabs.*;
import nl.lemval.nododue.util.NodoSetting;
import nl.lemval.nododue.util.SerialCommunicator;
import org.jdesktop.application.TaskService;

/**
 * The application's main frame.
 */
public class NodoDueManagerView extends FrameView implements StatusMessageListener {

    private ConnectPanel connectPanel;
    private MacroPanel macroPanel;
    private SettingsPanel settingsPanel;

    private final Timer messageTimer;
    private final Timer busyIconTimer;
    private final Icon idleIcon;
    private final Icon[] busyIcons = new Icon[15];
    private int busyIconIndex = 0;

    private JDialog aboutBox;
    private JDialog optionsBox;
    private JDialog dateTimeBox;
    private JDialog unitBox;
    private JDialog devicesBox;
    private CommandHistoryBox historyBox;

    public NodoDueManagerView(SingleFrameApplication app) {
	super(app);

	initComponents();

	ResourceMap resourceMap = getResourceMap();

	this.connectPanel = new ConnectPanel(this);
	this.macroPanel = new MacroPanel(this);
	this.settingsPanel = new SettingsPanel(this);

	tabContainer.addTab(resourceMap.getString("tab.connection"), connectPanel);
	tabContainer.addTab(resourceMap.getString("tab.nodoCommand"), new NodoCommandPanel(this));
	tabContainer.addTab(resourceMap.getString("tab.macroCommand"), macroPanel);
	tabContainer.addTab(resourceMap.getString("tab.kakuCommand"), new KakuCommandPanel(this));
	tabContainer.addTab(resourceMap.getString("tab.settings"), settingsPanel);
	tabContainer.addTab(resourceMap.getString("tab.monitor"), new MonitorPanel(this));
	tabContainer.addTab(resourceMap.getString("tab.raw"), new RawSignalPanel(this));

	showConnectMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_1, InputEvent.CTRL_MASK));
	showCommandMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_2, InputEvent.CTRL_MASK));
	showMacroMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_3, InputEvent.CTRL_MASK));
	showKakuMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_4, InputEvent.CTRL_MASK));
	showConfigMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_5, InputEvent.CTRL_MASK));
	showMonitorMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_6, InputEvent.CTRL_MASK));
	showRawSignalMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_7, InputEvent.CTRL_MASK));

        historyMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_D, InputEvent.CTRL_MASK));

	unitLabel.setBorder(new CompoundBorder(unitLabel.getBorder(), new EmptyBorder(2, 5, 2, 5)));
	statusMessageLabel.setText("");
	statusAnimationLabel.setVisible(false);
	getFrame().setResizable(false);

	// status bar initialization - message timeout, idle icon and busy animation, etc
	int messageTimeout = resourceMap.getInteger("StatusBar.messageTimeout");
	messageTimer = new Timer(messageTimeout, new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
		statusMessageLabel.setText("");
	    }
	});
	messageTimer.setRepeats(false);
	int busyAnimationRate = resourceMap.getInteger("StatusBar.busyAnimationRate");
	for (int i = 0; i < busyIcons.length; i++) {
	    busyIcons[i] = resourceMap.getIcon("StatusBar.busyIcons[" + i + "]");
	}
	busyIconTimer = new Timer(busyAnimationRate, new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
		busyIconIndex = (busyIconIndex + 1) % busyIcons.length;
		statusAnimationLabel.setIcon(busyIcons[busyIconIndex]);
	    }
	});
	idleIcon = resourceMap.getIcon("StatusBar.idleIcon");
	statusAnimationLabel.setIcon(idleIcon);
	progressBar.setVisible(false);

	// connecting action tasks to status bar via TaskMonitor
	TaskMonitor taskMonitor = new TaskMonitor(getApplication().getContext());
	taskMonitor.addPropertyChangeListener(new java.beans.PropertyChangeListener() {
	    public void propertyChange(java.beans.PropertyChangeEvent evt) {
		String propertyName = evt.getPropertyName();
		if ("started".equals(propertyName)) {
		    if (!busyIconTimer.isRunning()) {
			statusAnimationLabel.setIcon(busyIcons[0]);
			busyIconIndex = 0;
			busyIconTimer.start();
		    }
		    progressBar.setVisible(true);
		    progressBar.setIndeterminate(true);
		} else if ("done".equals(propertyName)) {
		    busyIconTimer.stop();
		    statusAnimationLabel.setIcon(idleIcon);
		    progressBar.setVisible(false);
		    progressBar.setValue(0);
		} else if ("message".equals(propertyName)) {
		    String text = (String) (evt.getNewValue());
		    statusMessageLabel.setText((text == null) ? "" : text);
		    messageTimer.restart();
		} else if ("progress".equals(propertyName)) {
		    int value = (Integer) (evt.getNewValue());
		    progressBar.setVisible(true);
		    progressBar.setIndeterminate(false);
		    progressBar.setValue(value);
		}
	    }
	});
    }

    @Action
    public void showAboutBox() {
	if (aboutBox == null) {
	    JFrame mainFrame = NodoDueManager.getApplication().getMainFrame();
	    aboutBox = new NodoDueManagerAboutBox(mainFrame);
	    aboutBox.setLocationRelativeTo(mainFrame);
	}
	NodoDueManager.getApplication().show(aboutBox);
    }

    @Action
    public void showHistoryBox() {
	if (historyBox == null) {
	    NodoDueManager app = NodoDueManager.getApplication();
	    JFrame mainFrame = app.getMainFrame();
	    historyBox = new CommandHistoryBox(mainFrame, false);
	    historyBox.setLocationRelativeTo(mainFrame);
	    SerialCommunicator comm = app.getSerialCommunicator();
	    historyBox.setHistory(comm.getHistory());
	}
	NodoDueManager.getApplication().show(historyBox);
    }

    @Action
    public void showOptionsBox() {
	if (optionsBox == null) {
	    JFrame mainFrame = NodoDueManager.getApplication().getMainFrame();
	    optionsBox = new NodoDueManagerOptionBox(mainFrame);
	    optionsBox.setLocationRelativeTo(mainFrame);
	}
	NodoDueManager.getApplication().show(optionsBox);
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        mainPanel = new javax.swing.JPanel();
        tabContainer = new javax.swing.JTabbedPane();
        menuBar = new javax.swing.JMenuBar();
        javax.swing.JMenu fileMenu = new javax.swing.JMenu();
        macroMenu = new javax.swing.JMenu();
        loadMacroMenuItem = new javax.swing.JMenuItem();
        saveMacroMenuItem = new javax.swing.JMenuItem();
        configMenu = new javax.swing.JMenu();
        loadConfigMenuItem = new javax.swing.JMenuItem();
        saveConfigMenuItem = new javax.swing.JMenuItem();
        jSeparator1 = new javax.swing.JPopupMenu.Separator();
        javax.swing.JMenuItem exitMenuItem = new javax.swing.JMenuItem();
        nodoMenu = new javax.swing.JMenu();
        unitMenuItem = new javax.swing.JMenuItem();
        devicesMenuItem = new javax.swing.JMenuItem();
        historyMenuItem = new javax.swing.JMenuItem();
        dateTimeMenuItem = new javax.swing.JMenuItem();
        learnMenuItem = new javax.swing.JMenuItem();
        schermMenu = new javax.swing.JMenu();
        showConnectMenuItem = new javax.swing.JMenuItem();
        showCommandMenuItem = new javax.swing.JMenuItem();
        showMacroMenuItem = new javax.swing.JMenuItem();
        showKakuMenuItem = new javax.swing.JMenuItem();
        showConfigMenuItem = new javax.swing.JMenuItem();
        showMonitorMenuItem = new javax.swing.JMenuItem();
        showRawSignalMenuItem = new javax.swing.JMenuItem();
        javax.swing.JMenu helpMenu = new javax.swing.JMenu();
        optionsMenuItem = new javax.swing.JMenuItem();
        jSeparator2 = new javax.swing.JPopupMenu.Separator();
        javax.swing.JMenuItem aboutMenuItem = new javax.swing.JMenuItem();
        statusPanel = new javax.swing.JPanel();
        javax.swing.JSeparator statusPanelSeparator = new javax.swing.JSeparator();
        statusMessageLabel = new javax.swing.JLabel();
        progressBar = new javax.swing.JProgressBar();
        connectStatus = new javax.swing.JButton();
        unitLabel = new javax.swing.JLabel();
        statusAnimationLabel = new javax.swing.JLabel();

        mainPanel.setMaximumSize(new java.awt.Dimension(800, 450));
        mainPanel.setMinimumSize(new java.awt.Dimension(800, 450));
        mainPanel.setName("mainPanel"); // NOI18N
        mainPanel.setPreferredSize(new java.awt.Dimension(800, 450));

        tabContainer.setName("kakuTab"); // NOI18N
        tabContainer.setPreferredSize(new java.awt.Dimension(725, 450));

        org.jdesktop.layout.GroupLayout mainPanelLayout = new org.jdesktop.layout.GroupLayout(mainPanel);
        mainPanel.setLayout(mainPanelLayout);
        mainPanelLayout.setHorizontalGroup(
            mainPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(tabContainer, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 800, Short.MAX_VALUE)
        );
        mainPanelLayout.setVerticalGroup(
            mainPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(mainPanelLayout.createSequentialGroup()
                .add(tabContainer, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        menuBar.setMaximumSize(new java.awt.Dimension(204, 21));
        menuBar.setMinimumSize(new java.awt.Dimension(204, 21));
        menuBar.setName("menuBar"); // NOI18N

        fileMenu.setMnemonic('B');
        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(nl.lemval.nododue.NodoDueManager.class).getContext().getResourceMap(NodoDueManagerView.class);
        fileMenu.setText(resourceMap.getString("fileMenu.text")); // NOI18N
        fileMenu.setName("fileMenu"); // NOI18N

        macroMenu.setText(resourceMap.getString("macroMenu.text")); // NOI18N
        macroMenu.setName("macroMenu"); // NOI18N

        javax.swing.ActionMap actionMap = org.jdesktop.application.Application.getInstance(nl.lemval.nododue.NodoDueManager.class).getContext().getActionMap(NodoDueManagerView.class, this);
        loadMacroMenuItem.setAction(actionMap.get("macroLoadFromFile")); // NOI18N
        loadMacroMenuItem.setText(resourceMap.getString("loadMacroMenuItem.text")); // NOI18N
        loadMacroMenuItem.setName("loadMacroMenuItem"); // NOI18N
        macroMenu.add(loadMacroMenuItem);

        saveMacroMenuItem.setAction(actionMap.get("macroSaveToFile")); // NOI18N
        saveMacroMenuItem.setText(resourceMap.getString("saveMacroMenuItem.text")); // NOI18N
        saveMacroMenuItem.setName("saveMacroMenuItem"); // NOI18N
        macroMenu.add(saveMacroMenuItem);

        fileMenu.add(macroMenu);

        configMenu.setLabel(resourceMap.getString("configMenu.label")); // NOI18N
        configMenu.setName("configMenu"); // NOI18N

        loadConfigMenuItem.setAction(actionMap.get("configLoadFromFile")); // NOI18N
        loadConfigMenuItem.setText(resourceMap.getString("loadConfigMenuItem.text")); // NOI18N
        loadConfigMenuItem.setName("loadConfigMenuItem"); // NOI18N
        configMenu.add(loadConfigMenuItem);

        saveConfigMenuItem.setAction(actionMap.get("configSaveToFile")); // NOI18N
        saveConfigMenuItem.setText(resourceMap.getString("saveConfigMenuItem.text")); // NOI18N
        saveConfigMenuItem.setName("saveConfigMenuItem"); // NOI18N
        configMenu.add(saveConfigMenuItem);

        fileMenu.add(configMenu);

        jSeparator1.setName("jSeparator1"); // NOI18N
        fileMenu.add(jSeparator1);

        exitMenuItem.setAction(actionMap.get("quit")); // NOI18N
        exitMenuItem.setText(resourceMap.getString("exitMenuItem.text")); // NOI18N
        exitMenuItem.setName("exitMenuItem"); // NOI18N
        fileMenu.add(exitMenuItem);

        menuBar.add(fileMenu);

        nodoMenu.setText(resourceMap.getString("nodoMenu.text")); // NOI18N
        nodoMenu.setName("nodoMenu"); // NOI18N

        unitMenuItem.setAction(actionMap.get("showUnitSelectorBox")); // NOI18N
        unitMenuItem.setText(resourceMap.getString("unitMenuItem.text")); // NOI18N
        unitMenuItem.setName("unitMenuItem"); // NOI18N
        nodoMenu.add(unitMenuItem);

        devicesMenuItem.setAction(actionMap.get("showDevicesBox")); // NOI18N
        devicesMenuItem.setText(resourceMap.getString("devicesMenuItem.text")); // NOI18N
        devicesMenuItem.setName("devicesMenuItem"); // NOI18N
        nodoMenu.add(devicesMenuItem);

        historyMenuItem.setAction(actionMap.get("showHistoryBox")); // NOI18N
        historyMenuItem.setText(resourceMap.getString("historyMenuItem.text")); // NOI18N
        historyMenuItem.setName("historyMenuItem"); // NOI18N
        nodoMenu.add(historyMenuItem);

        dateTimeMenuItem.setAction(actionMap.get("showDateTimeDialog")); // NOI18N
        dateTimeMenuItem.setText(resourceMap.getString("dateTimeMenuItem.text")); // NOI18N
        dateTimeMenuItem.setName("dateTimeMenuItem"); // NOI18N
        nodoMenu.add(dateTimeMenuItem);

        learnMenuItem.setAction(actionMap.get("showLearnCommandsDialog")); // NOI18N
        learnMenuItem.setText(resourceMap.getString("learnMenuItem.text")); // NOI18N
        learnMenuItem.setName("learnMenuItem"); // NOI18N
        nodoMenu.add(learnMenuItem);

        menuBar.add(nodoMenu);

        schermMenu.setText(resourceMap.getString("schermMenu.text")); // NOI18N
        schermMenu.setName("schermMenu"); // NOI18N

        showConnectMenuItem.setAction(actionMap.get("showConnectScreen")); // NOI18N
        showConnectMenuItem.setText(resourceMap.getString("showConnectMenuItem.text")); // NOI18N
        showConnectMenuItem.setToolTipText(resourceMap.getString("showConnectMenuItem.toolTipText")); // NOI18N
        showConnectMenuItem.setName("showConnectMenuItem"); // NOI18N
        schermMenu.add(showConnectMenuItem);

        showCommandMenuItem.setAction(actionMap.get("showCommandScreen")); // NOI18N
        showCommandMenuItem.setText(resourceMap.getString("showCommandMenuItem.text")); // NOI18N
        showCommandMenuItem.setToolTipText(resourceMap.getString("showCommandMenuItem.toolTipText")); // NOI18N
        showCommandMenuItem.setName("showCommandMenuItem"); // NOI18N
        schermMenu.add(showCommandMenuItem);

        showMacroMenuItem.setAction(actionMap.get("showMacroScreen")); // NOI18N
        showMacroMenuItem.setText(resourceMap.getString("showMacroMenuItem.text")); // NOI18N
        showMacroMenuItem.setName("showMacroMenuItem"); // NOI18N
        schermMenu.add(showMacroMenuItem);

        showKakuMenuItem.setAction(actionMap.get("showKakuScreen")); // NOI18N
        showKakuMenuItem.setText(resourceMap.getString("showKakuMenuItem.text")); // NOI18N
        showKakuMenuItem.setToolTipText(resourceMap.getString("showKakuMenuItem.toolTipText")); // NOI18N
        showKakuMenuItem.setName("showKakuMenuItem"); // NOI18N
        schermMenu.add(showKakuMenuItem);

        showConfigMenuItem.setAction(actionMap.get("showConfigScreen")); // NOI18N
        showConfigMenuItem.setText(resourceMap.getString("showConfigMenuItem.text")); // NOI18N
        showConfigMenuItem.setName("showConfigMenuItem"); // NOI18N
        schermMenu.add(showConfigMenuItem);

        showMonitorMenuItem.setAction(actionMap.get("showMonitorScreen")); // NOI18N
        showMonitorMenuItem.setText(resourceMap.getString("showMonitorMenuItem.text")); // NOI18N
        showMonitorMenuItem.setToolTipText(resourceMap.getString("showMonitorMenuItem.toolTipText")); // NOI18N
        showMonitorMenuItem.setName("showMonitorMenuItem"); // NOI18N
        schermMenu.add(showMonitorMenuItem);

        showRawSignalMenuItem.setAction(actionMap.get("showRawSignalScreen")); // NOI18N
        showRawSignalMenuItem.setText(resourceMap.getString("showRawSignalMenuItem.text")); // NOI18N
        showRawSignalMenuItem.setName("showRawSignalMenuItem"); // NOI18N
        schermMenu.add(showRawSignalMenuItem);

        menuBar.add(schermMenu);

        helpMenu.setText(resourceMap.getString("helpMenu.text")); // NOI18N
        helpMenu.setName("helpMenu"); // NOI18N

        optionsMenuItem.setAction(actionMap.get("showOptionsBox")); // NOI18N
        optionsMenuItem.setText(resourceMap.getString("optionsMenuItem.text")); // NOI18N
        optionsMenuItem.setName("optionsMenuItem"); // NOI18N
        helpMenu.add(optionsMenuItem);

        jSeparator2.setName("jSeparator2"); // NOI18N
        helpMenu.add(jSeparator2);

        aboutMenuItem.setAction(actionMap.get("showAboutBox")); // NOI18N
        aboutMenuItem.setText(resourceMap.getString("aboutMenuItem.text")); // NOI18N
        aboutMenuItem.setName("aboutMenuItem"); // NOI18N
        helpMenu.add(aboutMenuItem);

        menuBar.add(helpMenu);

        statusPanel.setMaximumSize(new java.awt.Dimension(740, 47));
        statusPanel.setName("statusPanel"); // NOI18N
        statusPanel.setPreferredSize(new java.awt.Dimension(740, 47));
        statusPanel.setRequestFocusEnabled(false);

        statusPanelSeparator.setName("statusPanelSeparator"); // NOI18N

        statusMessageLabel.setText(resourceMap.getString("statusMessageLabel.text")); // NOI18N
        statusMessageLabel.setName("statusMessageLabel"); // NOI18N

        progressBar.setName("progressBar"); // NOI18N

        connectStatus.setAction(actionMap.get("connectNodo")); // NOI18N
        connectStatus.setIcon(resourceMap.getIcon("connectStatus.icon")); // NOI18N
        connectStatus.setText(null);
        connectStatus.setToolTipText(resourceMap.getString("connectStatus.disabled.tooltip")); // NOI18N
        connectStatus.setDisabledIcon(new javax.swing.ImageIcon(getClass().getResource("/nl/lemval/nododue/resources/connect-base.png"))); // NOI18N
        connectStatus.setMargin(new java.awt.Insets(2, 2, 2, 2));
        connectStatus.setName("connectStatus"); // NOI18N
        connectStatus.setPressedIcon(new javax.swing.ImageIcon(getClass().getResource("/nl/lemval/nododue/resources/connect-base.png"))); // NOI18N

        unitLabel.setText(resourceMap.getString("unitLabel.text")); // NOI18N
        unitLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        unitLabel.setName("unitLabel"); // NOI18N
        unitLabel.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                unitFieldMouseClicked(evt);
            }
        });

        org.jdesktop.layout.GroupLayout statusPanelLayout = new org.jdesktop.layout.GroupLayout(statusPanel);
        statusPanel.setLayout(statusPanelLayout);
        statusPanelLayout.setHorizontalGroup(
            statusPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(statusPanelLayout.createSequentialGroup()
                .add(connectStatus, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 44, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(statusPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(statusPanelLayout.createSequentialGroup()
                        .add(4, 4, 4)
                        .add(unitLabel)
                        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                        .add(statusMessageLabel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 472, Short.MAX_VALUE)
                        .add(55, 55, 55)
                        .add(progressBar, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                        .addContainerGap())
                    .add(statusPanelSeparator, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 749, Short.MAX_VALUE)))
        );
        statusPanelLayout.setVerticalGroup(
            statusPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(org.jdesktop.layout.GroupLayout.TRAILING, statusPanelLayout.createSequentialGroup()
                .add(2, 2, 2)
                .add(statusPanelSeparator, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(statusPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(statusPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                        .add(unitLabel)
                        .add(statusMessageLabel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 19, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                    .add(progressBar, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .add(13, 13, 13))
            .add(statusPanelLayout.createSequentialGroup()
                .add(connectStatus)
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        statusAnimationLabel.setHorizontalAlignment(javax.swing.SwingConstants.LEFT);
        statusAnimationLabel.setName("statusAnimationLabel"); // NOI18N

        setComponent(mainPanel);
        setMenuBar(menuBar);
        setStatusBar(statusPanel);
    }// </editor-fold>//GEN-END:initComponents

    private void unitFieldMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_unitFieldMouseClicked
	showUnitSelectorBox();
    }//GEN-LAST:event_unitFieldMouseClicked

    public void showStatusMessage(String message) {
	statusMessageLabel.setText(message);
	connectPanel.handleOutputLine(message);
	messageTimer.restart();
    }

    @Action
    public void connectNodo() {
	connectPanel.connectSerial();
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JMenu configMenu;
    private javax.swing.JButton connectStatus;
    private javax.swing.JMenuItem dateTimeMenuItem;
    private javax.swing.JMenuItem devicesMenuItem;
    private javax.swing.JMenuItem historyMenuItem;
    private javax.swing.JPopupMenu.Separator jSeparator1;
    private javax.swing.JPopupMenu.Separator jSeparator2;
    private javax.swing.JMenuItem learnMenuItem;
    private javax.swing.JMenuItem loadConfigMenuItem;
    private javax.swing.JMenuItem loadMacroMenuItem;
    private javax.swing.JMenu macroMenu;
    private javax.swing.JPanel mainPanel;
    private javax.swing.JMenuBar menuBar;
    private javax.swing.JMenu nodoMenu;
    private javax.swing.JMenuItem optionsMenuItem;
    private javax.swing.JProgressBar progressBar;
    private javax.swing.JMenuItem saveConfigMenuItem;
    private javax.swing.JMenuItem saveMacroMenuItem;
    private javax.swing.JMenu schermMenu;
    private javax.swing.JMenuItem showCommandMenuItem;
    private javax.swing.JMenuItem showConfigMenuItem;
    private javax.swing.JMenuItem showConnectMenuItem;
    private javax.swing.JMenuItem showKakuMenuItem;
    private javax.swing.JMenuItem showMacroMenuItem;
    private javax.swing.JMenuItem showMonitorMenuItem;
    private javax.swing.JMenuItem showRawSignalMenuItem;
    private javax.swing.JLabel statusAnimationLabel;
    private javax.swing.JLabel statusMessageLabel;
    private javax.swing.JPanel statusPanel;
    private javax.swing.JTabbedPane tabContainer;
    private javax.swing.JLabel unitLabel;
    private javax.swing.JMenuItem unitMenuItem;
    // End of variables declaration//GEN-END:variables

    public void readyForConnection(boolean state) {
	connectStatus.setEnabled(state);
	if (state) {
	    connectStatus.setIcon(getResourceMap().getIcon("connectStatus.disconnect.icon")); // NOI18N
	    connectStatus.setToolTipText(getResourceMap().getString("connectStatus.disconnect.tooltip"));
	} else {
	    connectStatus.setIcon(getResourceMap().getIcon("connectStatus.icon")); // NOI18N
	    connectStatus.setToolTipText(getResourceMap().getString("connectStatus.disabled.tooltip"));
	}
    }

    public void connected(boolean valid) {
	connectStatus.setIcon(getResourceMap().getIcon("connectStatus.connect.icon")); // NOI18N
	connectStatus.setToolTipText(getResourceMap().getString("connectStatus.connect.tooltip"));
	if (valid) {
	    updateUnitField();
	}
    }

    public void disconnected() {
	connectStatus.setIcon(getResourceMap().getIcon("connectStatus.disconnect.icon")); // NOI18N
	connectStatus.setToolTipText(getResourceMap().getString("connectStatus.disconnect.tooltip"));
    }

    @Action
    public void macroLoadFromFile() {
	File selected = selectFile(false, ".mac", "macro_file_description");
	if (selected != null && selected.exists()) {
	    Options.getInstance().setFolder(selected.getParent());
	    macroPanel.loadFromFile(selected);
	}
    }

    @Action
    public void macroSaveToFile() {
	File selected = selectFile(true, ".mac", "macro_file_description");
	if (selected != null) {
	    if (selected.getName().endsWith(".mac") == false) {
		selected = new File(selected.getPath() + ".mac");
	    }
	    Options.getInstance().setFolder(selected.getParent());
	    boolean overwrite = checkOverwrite(selected);
	    macroPanel.saveToFile(selected, overwrite);
	}
    }

    @Action
    public void configLoadFromFile() {
	File selected = selectFile(false, ".ndc", "config_file_description");
	if (selected == null || !selected.exists()) {
	    return;
	}

	Options.getInstance().setFolder(selected.getParent());
	Collection<NodoSetting> settings = NodoSetting.loadFromFile(selected);
	settingsPanel.updateSettings(settings);
    }

    @Action
    public void configSaveToFile() {
	Collection<NodoSetting> settings = settingsPanel.getSettings();
	if (settings == null || settings.isEmpty()) {
	    // Ask retrieve settings from Nodo first....
	    JOptionPane.showMessageDialog(this.getFrame(),
		    getResourceMap().getString("saveConfig.message"),
		    getResourceMap().getString("saveConfig.title"),
		    JOptionPane.WARNING_MESSAGE);
	    return;
	}

	File selected = selectFile(true, ".ndc", "config_file_description");
	if (selected != null) {
	    if (selected.getName().endsWith(".ndc") == false) {
		selected = new File(selected.getPath() + ".ndc");
	    }
	    Options.getInstance().setFolder(selected.getParent());
	    boolean overwrite = checkOverwrite(selected);
	    NodoSetting.saveToFile(settings, selected, overwrite);
	}
    }

    private boolean checkOverwrite(File selected) {
	if (selected.exists() == false) {
	    return false;
	}

	Object[] options = {
	    getResourceMap().getString("fileSelection.overwrite.yes"),
	    getResourceMap().getString("fileSelection.overwrite.no")
	};
	int n = JOptionPane.showOptionDialog(this.getFrame(),
		getResourceMap().getString("fileSelection.overwrite.question", selected.getName()),
		getResourceMap().getString("fileSelection.overwrite.title"),
		JOptionPane.YES_NO_OPTION,
		JOptionPane.QUESTION_MESSAGE,
		null,
		options,
		options[1]);
	switch (n) {
	    case JOptionPane.YES_OPTION:
		return true;
	    case JOptionPane.NO_OPTION:
	    case JOptionPane.CLOSED_OPTION:
	    case JOptionPane.CANCEL_OPTION:
	    default:
		break;
	}
	return false;
    }

    private File selectFile(boolean saveMode, final String ext, final String desc) {
	String base = Options.getInstance().getFolder();
	JFileChooser fc = new JFileChooser(base == null ? null : new File(base));
	fc.setFileFilter(new FileFilter() {
	    @Override
	    public boolean accept(File f) {
		return (f.isDirectory() || f.getName().endsWith(ext));
	    }

	    @Override
	    public String getDescription() {
		return getResourceMap().getString(desc);
	    }
	});
	int returnVal = saveMode
		? fc.showSaveDialog(this.getComponent())
		: fc.showOpenDialog(this.getComponent());

	if (returnVal == JFileChooser.APPROVE_OPTION) {
	    return fc.getSelectedFile();
	}
	return null;
    }

    void initialize() {
	TaskService ts = getApplication().getContext().getTaskService();
	ts.execute(connectPanel.scanPorts());
	tabContainer.setSelectedIndex(0);
    }

    @Action
    public void showConnectScreen() {
	tabContainer.setSelectedIndex(0);
    }

    @Action
    public void showCommandScreen() {
	tabContainer.setSelectedIndex(1);
    }

    @Action
    public void showMacroScreen() {
	tabContainer.setSelectedIndex(2);
    }

    @Action
    public void showKakuScreen() {
	tabContainer.setSelectedIndex(3);
    }

    @Action
    public void showConfigScreen() {
	tabContainer.setSelectedIndex(4);
    }

    @Action
    public void showMonitorScreen() {
	tabContainer.setSelectedIndex(5);
    }

    @Action
    public void showRawSignalScreen() {
	tabContainer.setSelectedIndex(6);
    }

    @Action
    public void showUnitSelectorBox() {
	showUnitSelectionBox();
	updateUnitField();
    }

    private void updateUnitField() {
	Options options = Options.getInstance();
	String unit = "???";
	if (options.isUseLocalUnit() && !options.isUseRemoteUnits()) {
	    unit = String.valueOf(options.getNodoUnit());
	} else {
	    unit = (options.isUseLocalUnit() ? options.getNodoUnit() + "; " : "")
		    + Arrays.toString(options.getRemoteUnits());
	}
	unitLabel.setText(getResourceMap().getString("unitLabel.info", unit));
    }

    @Action
    public void showDateTimeDialog() {
	if (dateTimeBox == null) {
	    JFrame mainFrame = NodoDueManager.getApplication().getMainFrame();
	    dateTimeBox = new ConfigDateBox(mainFrame, false);
	    dateTimeBox.setLocationRelativeTo(mainFrame);
	}
	NodoDueManager.getApplication().show(dateTimeBox);
    }

    @Action
    public void showDevicesBox() {
	if (devicesBox == null) {
	    JFrame mainFrame = NodoDueManager.getApplication().getMainFrame();
	    devicesBox = new DevicesBox(mainFrame, false);
	    devicesBox.setLocationRelativeTo(mainFrame);
	}
	NodoDueManager.getApplication().show(devicesBox);
    }

    public void showUnitSelectionBox() {
	if (unitBox == null) {
	    JFrame mainFrame = NodoDueManager.getApplication().getMainFrame();
	    unitBox = new UnitSelectorBox(mainFrame);
	    unitBox.setLocationRelativeTo(mainFrame);
	}
	NodoDueManager.getApplication().show(unitBox);
    }

    @Action
    public void showLearnCommandsDialog() {
	    JFrame mainFrame = NodoDueManager.getApplication().getMainFrame();
	    JDialog dialog = new LearnCommandBox(mainFrame, true);
	    dialog.setLocationRelativeTo(mainFrame);
        NodoDueManager.getApplication().show(dialog);
    }
}
