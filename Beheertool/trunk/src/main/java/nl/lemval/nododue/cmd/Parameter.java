/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.cmd;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import nl.lemval.nododue.util.listeners.HexKeyListener;

/**
 * This class represents a value from the spreadsheet and
 * contains the name and all possible values for a certain
 * command parameter. It is a meta class; it does not contain
 * any actual selections.
 * 
 * @author Michael
 */
public class Parameter {

    private class Prop {
        public String name;
        public String value;
        public Prop(String n, String v) { this.name = n.trim(); this.value = v.trim(); }
    };

    private String name;
    private String[] values = null;
    private String[] valueNames = null;
    private String validator;
    private boolean notNull = false;

    private static HexKeyListener kl = new HexKeyListener();

    Parameter(String name, String values) {
        prepare(values);
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public String[] getValueNames() {
        if ( valueNames == null && validator != null ) {
            // Retrieve all commands of type 'validator'
            // and add as option.
            ArrayList<String> vals = new ArrayList<String>();
            for(CommandInfo ci : CommandLoader.getActions(CommandType.fromString(validator)) ) {
                vals.add(ci.getName());
            }
            valueNames = values = vals.toArray(new String[0]);
        }
        return valueNames;
    }

    /**
     * Retrieve the actual value for a valueName. So if the parameter
     * was specified as "Off:1", a value of "Off" returns "1";
     * If there is no mapping, the name is checked for a decimal or
     * hexidecimal value. The latter one being converted to decimal
     * and the first one taking precedence. Also correct for notNull.
     *
     * @param parValue The named value
     * @return The value represented by this name
     */
    public String getValue(String parValue) {
        // First check for a named value.
	getValueNames();
        for (int i = 0; i < valueNames.length; i++) {
            if (valueNames[i].equals(parValue))
                return values[i];
        }
        for (int i = 0; i < values.length; i++) {
            if (values[i].equals(parValue))
                return values[i];
        }
        String rv = kl.toDecValue(parValue);
	if ( notNull && "0".equals(rv) ) {
	    return null;
	}
	return rv;
    }
    
    public String getValueName(String value) {
	getValueNames();
	for (int i = 0; i < values.length; i++) {
            if ( values[i].equals(value) )
                return valueNames[i];
        }
        return value;
    }

    private void prepare(String input) {
        // Check for reference value
        if ( input.matches("\\[.*\\]") ) {
            validator = input.substring(1, input.length()-1);
	    // Initialize the arrays.
	    // Delay until realy needed ;-) otherwise the list is not fully loaded.
	    // getValueNames();
            return;
        }

	if ( input.equals("!0")  ) {
	    notNull = true;
	    valueNames = new String[0];
	    values = new String[0];
	    return;
	}

        // Nope, check for multiple values, comma separated
        String[] set = input.split(",");

        // Make sure it is sorted by using a tree set
        TreeSet<Prop> list = new TreeSet<Prop>(new Comparator<Prop>() {
            public int compare(Prop o1, Prop o2) {
                String n1 = o1.name;
                String n2 = o2.name;
                try {
                    Integer.parseInt(n1);
                    n1 = "000".substring(n1.length()) + n1;
                } catch (NumberFormatException e) {
                    // This one is a string...
                }
                try {
                    Integer.parseInt(n2);
                    n2 = "000".substring(n2.length()) + n2;
                } catch (NumberFormatException e) {
                    // This one is also a string...
                }
                return n1.compareTo(n2);
            }
        });

        Pattern rangePattern = Pattern.compile("\\[?([0-9]+)\\.\\.([0-9]+)\\]?");
        Pattern namedPattern = Pattern.compile("([^:]+):(.+)");

        // Ok, now split it up
        for (int i = 0; i < set.length; i++) {
            String value = set[i];
            Matcher rangeMatcher = rangePattern.matcher(value);
            Matcher namedMatcher = namedPattern.matcher(value);
            if ( rangeMatcher.matches() ) {
                int start = Integer.parseInt(rangeMatcher.group(1));
                int end = Integer.parseInt(rangeMatcher.group(2));
                if ( end < start ) {
                    int tmp = start; start = end; end = tmp;
                }
                for (int j = start; j <= end; j++) {
                    String data = String.valueOf(j);
                    list.add(new Prop(data, data));
                }
            } else if ( namedMatcher.matches() ) {
                list.add(new Prop(namedMatcher.group(1), namedMatcher.group(2)));
            } else if ( value.trim().length() > 0 ) {
                list.add(new Prop(value, value));
            }
        }

        valueNames = new String[list.size()];
        values = new String[list.size()];

        int i = 0;
        for (Prop prop : list) {
            valueNames[i] = prop.name;
            values[i] = prop.value;
            i++;
        }
    }
}
