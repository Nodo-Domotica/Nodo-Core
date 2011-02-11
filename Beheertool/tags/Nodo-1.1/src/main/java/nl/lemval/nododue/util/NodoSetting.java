/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.CommandType;
import nl.lemval.nododue.cmd.Parameter;

/**
 * A NodoSetting is a class representing a configuration setting of the Nodo
 * in which the CommandInfo represents the command and the attributes the
 * actual name/value pairs reflecting the current setting value(s).
 * 
 * @author Michael
 */
public class NodoSetting {

    private CommandInfo info;
    private String attributeName1;
    private String attributeValue1;
    private String attributeData1;
    private String attributeName2;
    private String attributeValue2;
    private String attributeData2;
    private boolean hasSecondAttribute = false;

    public NodoSetting(CommandInfo setting) {
        this.info = setting;
        try {
            this.attributeName1 = setting.getParameter(0).getName();
            this.attributeName2 = setting.getParameter(1).getName();
            if ( attributeName2 != null && attributeName2.length() > 0 ) {
                hasSecondAttribute = true;
            }
        } catch (Exception e) {
            NodoDueManager.showMessage("NodoSetting.invalid",
                    (setting==null? "-" : setting.getName()), e.getMessage(), e);
        }
    }

    @Override
    public String toString() {
	StringBuilder builder = new StringBuilder("Setting ");
	builder.append(info.getName());
	builder.append(": ");
	builder.append(attributeName1);
	builder.append(" = ");
	builder.append(attributeData1);
	builder.append(" (");
	builder.append(attributeValue1);
	builder.append("); ");
	if ( hasSecondAttribute ) {
	    builder.append(attributeName2);
	    builder.append(" = ");
	    builder.append(attributeData2);
	    builder.append(" (");
	    builder.append(attributeValue2);
	    builder.append("); ");
	}
	return builder.toString();
    }

    public boolean isCommand() {
        return info.getType().is(CommandType.COMMAND);
    }

    public CommandInfo getInfo() {
	return info;
    }

    public boolean hasSecondAttribute() {
        return hasSecondAttribute;
    }

    public boolean firstAttributeIsFixed() {
        return info.hasQueryRange();
    }

    public String getAttributeName1() {
        return attributeName1;
    }

    public String getAttributeName2() {
        return attributeName2;
    }

    public String getAttributeValue1() {
        return attributeValue1;
    }

    public void setAttributeValue1(String data) {
	Parameter parm = info.getParameter(0);
        this.attributeValue1 = parm.getValueName(data);
        this.attributeData1 = parm.getValue(data);
    }

    public void setAttributeValue2(String data) {
	if ( hasSecondAttribute ) {
	    Parameter parm = info.getParameter(1);
	    this.attributeValue2 = parm.getValueName(data);
	    this.attributeData2 = parm.getValue(data);
	}
    }

    public String getAttributeValue2() {
        return attributeValue2;
    }

    public String getAttributeData1() {
        return attributeData1;
    }

    public String getAttributeData2() {
        return attributeData2;
    }

    public String getName() {
        return info.getName();
    }

    public void parseValues(String message) {
        try {
            String[] values = message.split(",");
            this.attributeData1 = values[0];
            this.attributeValue1 =
                info.getParameter(0).getValueName(values[0]);
            if ( hasSecondAttribute ) {
                this.attributeData2 = values[1];
                this.attributeValue2 =
                    info.getParameter(1).getValueName(values[1]);
            }
        } catch (Exception e) {
            System.out.println("Fail on split of '"+message+"'");
        }
    }

    public static Collection<NodoSetting> loadFromFile(File selected) {
        HashMap<String, CommandInfo> col = new HashMap<String, CommandInfo>();
        Collection<CommandInfo> elems = CommandLoader.getActions(CommandType.ALL);
        for (CommandInfo commandInfo : elems) {
            col.put(commandInfo.getName(), commandInfo);
        }

        HashSet<NodoSetting> settings = new HashSet<NodoSetting>();
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new FileReader(selected));
            String line;
            while ( (line = reader.readLine()) != null ) {
                if ( line.startsWith("#") == false ) {
                    String[] data = line.split(" ");
                    CommandInfo cmd = col.get(data[0]);
                    if ( cmd != null ) {
                        NodoSetting setting = new NodoSetting(cmd);
                        setting.parseValues(data[1]);
                        settings.add(setting);
                    }
                }
            }
            return settings;
        } catch (IOException e) {
            NodoDueManager.showDialog("NodoSetting.load_failed", selected, e.getMessage(), e);
        } finally {
            try { reader.close(); } catch (Exception e) {}
        }
        return settings;
    }

    public static void saveToFile(Collection<NodoSetting> data, File selected, boolean overwrite) {
        if ( selected.exists() && !overwrite ) {
            selected.renameTo(new File(selected.getPath()+".bak"));
        }
        ArrayList<NodoSetting> sortedData = new ArrayList<NodoSetting>();
        sortedData.addAll(data);
        Collections.sort(sortedData, new Comparator<NodoSetting>() {
            public int compare(NodoSetting o1, NodoSetting o2) {
                int rv = o1.getName().compareTo(o2.getName());
                if ( rv == 0 ) {
                    rv = o1.getAttributeValue1().compareTo(o2.getAttributeValue1());
                }
                return rv;
            }
        });

        String NEWLINE = System.getProperty("line.separator");
        BufferedWriter writer = null;
        try {
            writer = new BufferedWriter(new FileWriter(selected));
            writer.write("# Export " + SimpleDateFormat.getDateTimeInstance().format(new Date()));
            writer.write(NEWLINE);
            for (NodoSetting setting : sortedData) {
                writer.write(setting.getName());
                writer.write(" ");
                writer.write(setting.getAttributeData1());
                if ( setting.hasSecondAttribute ) {
                    writer.write(",");
                    writer.write(setting.getAttributeData2());
                }
                writer.write(NEWLINE);
            }
        } catch (IOException e) {
            NodoDueManager.showDialog("NodoSetting.save_failed", selected, e.getMessage(), e);
        } finally {
            try { writer.close(); } catch (Exception e) {}
        }
    }

    @Override
    public int hashCode() {
        int hash = 3;
        hash = 17 * hash + (this.info != null ? this.info.getName().hashCode() : 0);
        hash = 17 * hash + (this.attributeName1 != null ? this.attributeName1.hashCode() : 0);
        hash = 17 * hash + (this.attributeValue1 != null ? this.attributeValue1.hashCode() : 0);
        hash = 17 * hash + (this.attributeData1 != null ? this.attributeData1.hashCode() : 0);
        hash = 17 * hash + (this.attributeName2 != null ? this.attributeName2.hashCode() : 0);
        hash = 17 * hash + (this.attributeValue2 != null ? this.attributeValue2.hashCode() : 0);
        hash = 17 * hash + (this.attributeData2 != null ? this.attributeData2.hashCode() : 0);
        hash = 17 * hash + (this.hasSecondAttribute ? 1 : 0);
        return hash;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final NodoSetting other = (NodoSetting) obj;
        if (this.info != other.info && (this.info == null || !this.info.getName().equals(other.info.getName()))) {
            return false;
        }
        if ((this.attributeName1 == null) ? (other.attributeName1 != null) : !this.attributeName1.equals(other.attributeName1)) {
            return false;
        }
        if ((this.attributeValue1 == null) ? (other.attributeValue1 != null) : !this.attributeValue1.equals(other.attributeValue1)) {
            return false;
        }
        if ((this.attributeData1 == null) ? (other.attributeData1 != null) : !this.attributeData1.equals(other.attributeData1)) {
            return false;
        }
        if ((this.attributeName2 == null) ? (other.attributeName2 != null) : !this.attributeName2.equals(other.attributeName2)) {
            return false;
        }
        if ((this.attributeValue2 == null) ? (other.attributeValue2 != null) : !this.attributeValue2.equals(other.attributeValue2)) {
            return false;
        }
        if ((this.attributeData2 == null) ? (other.attributeData2 != null) : !this.attributeData2.equals(other.attributeData2)) {
            return false;
        }
        if (this.hasSecondAttribute != other.hasSecondAttribute) {
            return false;
        }
        return true;
    }
}
