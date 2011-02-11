/*
 * NodoDueManager.java
 */
package nl.lemval.nododue;

import java.awt.Font;
import nl.lemval.nododue.util.SerialCommunicator;
import java.text.MessageFormat;
import java.util.EventObject;
import java.util.ResourceBundle;
import javax.swing.ImageIcon;
import javax.swing.JOptionPane;
import javax.swing.UIManager;
import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;

/**
 * The main class of the application.
 */
public class NodoDueManager extends SingleFrameApplication {

    private SerialCommunicator communicator = null;
    private NodoDueManagerView view = null;
    private static ResourceBundle bundle = ResourceBundle.getBundle("application");
    protected static boolean linux = false;

    public NodoDueManager() {
    }

    /**
     * At startup create and show the main frame of the application.
     */
    @Override
    protected void startup() {
	// Set swing defaults
	setSwingDefaults();
	// Load the configuration
	Options.getInstance();

	// Show the application
	view = new NodoDueManagerView(this);
	ImageIcon icon = new ImageIcon(getClass().getResource("/nl/lemval/nododue/resources/ndb16.png"));
	view.getFrame().setIconImage(icon.getImage());
	show(view);
	view.initialize();
    }

    @Override
    public void exit(EventObject event) {
	getSerialCommunicator().disconnect();
	Options.getInstance().save();
	super.exit(event);
    }

    public NodoDueManagerView getView() {
	return view;
    }

    public static String getMessage(String key, Object... obj) {
	String format = bundle.getString(key);
	return MessageFormat.format(format, obj);
    }

    public static void showMessage(String key, Object... obj) {
	getApplication().getView().showStatusMessage(getMessage(key, obj));
	if (obj != null) {
	    for (int i = 0; i < obj.length; i++) {
		if (obj[i] != null && obj[i] instanceof Exception) {
		    ((Exception) obj[i]).printStackTrace();
		}
	    }
	}
    }

    public static void showDialog(String key, Object... obj) {
	JOptionPane.showMessageDialog(getApplication().getView().getComponent(), getMessage(key, obj), getMessage("warningTitle"), JOptionPane.WARNING_MESSAGE);
	if (obj != null) {
	    for (int i = 0; i < obj.length; i++) {
		if (obj[i] != null && obj[i] instanceof Exception) {
		    ((Exception) obj[i]).printStackTrace();
		}
	    }
	}
    }

    public static boolean hasConnection() {
	return getApplication().hasConnection(true);
    }

    public boolean hasConnection(boolean showWarning) {
	if (getSerialCommunicator().isListening()) {
	    return true;
	}
	if (showWarning) {
	    String msg = view.getResourceMap().getString("comm.notavailable.message");
	    String title = view.getResourceMap().getString("comm.notavailable.title");
	    JOptionPane.showMessageDialog(view.getComponent(), msg, title, JOptionPane.WARNING_MESSAGE);
	}
	return false;
    }

    public void reportTimeout() {
	String msg = view.getResourceMap().getString("comm.timeout.message");
	String title = view.getResourceMap().getString("comm.timeout.title");
	JOptionPane.showMessageDialog(view.getComponent(), msg, title, JOptionPane.WARNING_MESSAGE);
    }

    public static boolean isLinux() {
	return linux;
    }


    public SerialCommunicator getSerialCommunicator() {
	if (communicator == null) {
	    communicator = new SerialCommunicator();
	}
	return communicator;
    }

    /**
     * This method is to initialize the specified window by injecting resources.
     * Windows shown in our application come fully initialized from the GUI
     * builder, so this additional configuration is not needed.
     */
    @Override
    protected void configureWindow(java.awt.Window root) {
    }

    private void setSwingDefaults() {
	Font font = Options.getInstance().getBaseFont();
	UIManager.put("JMenuBar.font", font);
	UIManager.put("JMenuItem.font", font);
	UIManager.put("JButton.font", font);
	UIManager.put("JToggleButton.font", font);
	UIManager.put("OptionPane.messageFont", font);
	UIManager.put("JRadioButton.font", font);
	UIManager.put("JCheckBox.font", font);
	UIManager.put("JColorChooser.font", font);
	UIManager.put("JComboBox.font", font);
	UIManager.put("JLabel.font", font);
	UIManager.put("JList.font", font);
	UIManager.put("JRadioButtonMenuItem.font", font);
	UIManager.put("JCheckBoxMenuItem.font", font);
	UIManager.put("JMenu.font", font);
	UIManager.put("JPopupMenu.font", font);
	UIManager.put("JOptionPane.font", font);
	UIManager.put("JPanel.font", font);
	UIManager.put("JProgressBar.font", font);
	UIManager.put("JScrollPane.font", font);
	UIManager.put("JViewport.font", font);
	UIManager.put("JTabbedPane.font", font);
	UIManager.put("JTable.font", font);
	UIManager.put("TableHeader.font", font);
	UIManager.put("JTextField.font", font);
	UIManager.put("JPasswordField.font", font);
	UIManager.put("JTextArea.font", font);
	UIManager.put("JTextPane.font", font);
	UIManager.put("JEditorPane.font", font);
	UIManager.put("JTitledBorder.font", font);
	UIManager.put("JToolBar.font", font);
	UIManager.put("JToolTip.font", font);
	UIManager.put("JTree.font", font);
	UIManager.put("Button.font", font);
	UIManager.put("ToggleButton.font", font);
	UIManager.put("RadioButton.font", font);
	UIManager.put("CheckBox.font", font);
	UIManager.put("ColorChooser.font", font);
	UIManager.put("ComboBox.font", font);
	UIManager.put("Label.font", font);
	UIManager.put("List.font", font);
	UIManager.put("MenuBar.font", font);
	UIManager.put("MenuItem.font", font);
	UIManager.put("RadioButtonMenuItem.font", font);
	UIManager.put("CheckBoxMenuItem.font", font);
	UIManager.put("Menu.font", font);
	UIManager.put("PopupMenu.font", font);
	UIManager.put("OptionPane.font", font);
	UIManager.put("Panel.font", font);
	UIManager.put("ProgressBar.font", font);
	UIManager.put("ScrollPane.font", font);
	UIManager.put("Viewport.font", font);
	UIManager.put("TabbedPane.font", font);
	UIManager.put("Table.font", font);
	UIManager.put("TableHeader.font", font);
	UIManager.put("TextField.font", font);
	UIManager.put("PasswordField.font", font);
	UIManager.put("TextArea.font", font);
	UIManager.put("TextPane.font", font);
	UIManager.put("EditorPane.font", font);
	UIManager.put("TitledBorder.font", font);
	UIManager.put("ToolBar.font", font);
	UIManager.put("ToolTip.font", font);
	UIManager.put("Tree.font", font);
    }

    /**
     * A convenient static getter for the application instance.
     * @return the instance of NodoDueManager
     */
    public static NodoDueManager getApplication() {
	return Application.getInstance(NodoDueManager.class);
    }

}
