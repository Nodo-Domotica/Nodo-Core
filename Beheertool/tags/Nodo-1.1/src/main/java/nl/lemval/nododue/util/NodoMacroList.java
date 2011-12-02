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
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Set;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.Options;
import nl.lemval.nododue.cmd.CommandDeviceInfo;
import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.CommandType;

/**
 *
 * @author Michael
 */
public class NodoMacroList {

    public static final int MAXLENGTH = 120;
    
    private ArrayList<NodoMacro> macros;
    
    public NodoMacroList() {
        macros = new ArrayList<NodoMacro>();
    }

    public boolean add(int idx, NodoMacro macro) {
        if ( macros.size() < MAXLENGTH-1 ) {
            return macros.add(macro);
        }
        return false;
    }

    @SuppressWarnings("element-type-mismatch")
    public boolean remove(Object o) {
        return macros.remove(o);
    }

    public int size() {
        return macros.size();
    }

    public NodoMacro get(int index) {
        if ( index >= 0 && index < macros.size() )
            return macros.get(index);
        return null;
    }

    public boolean move(int index, boolean up) {
        int len = size();
        // If it needs to be moved down, it should be at most the
        // before last item.
        if (!up) len--;

        if ( index >= len ) {
            return false;
        }
        NodoMacro elem = macros.get(index);
        if ( elem == null ) {
            return false;
        }

        int swindex = (up ? index-1 : index+1);
        NodoMacro swelem = macros.get(swindex);
        macros.remove(index);
        macros.add(index, swelem);
        macros.remove(swindex);
        macros.add(swindex, elem);
        return true;
    }
    
    public boolean add(NodoMacro nodoMacro) {
	if ( size() >= MAXLENGTH ) {
	    return false;
	}
        for (NodoMacro elem : macros) {
           if ( elem.equals(nodoMacro) ) {
               return false;
           }
        }
        return macros.add(nodoMacro);
    }

    public boolean load(File selected) {
        HashMap<String, CommandInfo> col = new HashMap<String, CommandInfo>();
        Collection<CommandInfo> elems = CommandLoader.getActions(CommandType.ALL);
        for (CommandInfo commandInfo : elems) {
            col.put(commandInfo.getName(), commandInfo);
        }
        Set<Device> applicances = Options.getInstance().getApplicances();
        HashMap<String, Device> dev = new HashMap<String, Device>();
        for (Device device : applicances) {
            if ( device.isActive() ) {
                dev.put(device.getSignal(), device);
            }
        }

        BufferedReader reader = null;
	boolean loadAll = true;
        try {
            macros.clear();
            reader = new BufferedReader(new FileReader(selected));
            String line;
            while ( (line = reader.readLine()) != null ) {
                NodoMacro macro = NodoMacro.loadFrom(col, dev, line);
                if ( macro != null ) {
                    loadAll = loadAll && add(macro);
		}
            }
	    if (!loadAll) {
                NodoDueManager.showDialog("Macroloader.load_incomplete");
	    }
            return true;
        } catch (IOException e) {
            NodoDueManager.showDialog("Macroloader.load_failed", selected, e.getMessage(), e);
        } finally {
            try { reader.close(); } catch (Exception e) {}
        }
        return false;
    }

    public void save(File selected, boolean overwrite) {
        if ( selected.exists() && !overwrite ) {
            selected.renameTo(new File(selected.getPath()+".bak"));
        }
        String NEWLINE = System.getProperty("line.separator");
        BufferedWriter writer = null;
        try {
            writer = new BufferedWriter(new FileWriter(selected));
            for (NodoMacro nodoMacro : macros) {
                writer.write(nodoMacro.getSaveFormat());
                writer.write(NEWLINE);
            }
        } catch (IOException e) {
            NodoDueManager.showDialog("Macroloader.save_failed", selected, e.getMessage(), e);
        } finally {
            try { writer.close(); } catch (Exception e) {}
        }
    }
}
