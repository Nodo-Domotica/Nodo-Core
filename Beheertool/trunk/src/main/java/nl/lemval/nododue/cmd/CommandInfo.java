/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.cmd;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * This is the class describing a command, not the actual command.
 * 
 * @author Michael
 */
public class CommandInfo {

    private String name;
    private Name nameElement;
    private String description;
    private String explanation;
    private ArrayList<Parameter> parameters;
    private CommandType type;
    private int[] queryRange;

    public enum State {

        On,
        Off,
    };

    public enum Name {

        EventlistErase,
        EventlistShow,
        EventlistWrite,
        SendKAKU,
        SendNewKAKU,
        Unit,
        Status,
        KAKU,
        ClockSetDOW,
        ClockSetYear,
        ClockSetDate,
        ClockSetTime,
        RawsignalGet,
        RawsignalPut,
        TimerSetMin,
        TimerSetSec,
        SendUserEvent,
        UserEvent,
        VariableSet,
        WiredOut,
        WiredPullup,
        WiredSmittTrigger,
        WiredThreshold,
        WiredAnalog,
        WiredRange,
    };
    public static final String CUSTOM_CMD = "<Custom>";
    public static final Collection<String> UNIT_SET = Arrays.asList(new String[]{
                Name.Unit.name()});
    public static final Collection<String> DATE_SET = Arrays.asList(new String[]{
                Name.ClockSetDOW.name(),
                Name.ClockSetDate.name(),
                Name.ClockSetTime.name(),
                Name.ClockSetYear.name()});
    public static final Collection<String> VAR_SET = Arrays.asList(new String[]{
                Name.VariableSet.name()});
    public static final Collection<String> TIMER_SET = Arrays.asList(new String[]{
                Name.TimerSetSec.name(),
                Name.TimerSetMin.name()});
    public static final Collection<String> WIRE_SET = Arrays.asList(new String[]{
                Name.WiredSmittTrigger.name(),
                Name.WiredThreshold.name(),
                Name.WiredRange.name(),
                Name.WiredPullup.name()});
    public static final Collection<String> OUT_SET = Arrays.asList(new String[]{
                Name.WiredOut.name()});

    public CommandInfo(String name, String description, String remark) {
        this.name = name;
        try {
            this.nameElement = Name.valueOf(name);
        } catch (IllegalArgumentException e) {
        }
        this.description = description;
        this.explanation = remark;
        this.parameters = new ArrayList<Parameter>();
        this.type = CommandType.NOTHING;
    }

    public void addParameter(String name, String values) {
        parameters.add(new Parameter(name, values));
    }

    public Parameter getParameter(int i) {
        if (i >= parameters.size()) {
            return null;
        }
        return parameters.get(i);
    }

    public String getDescription() {
        return description;
    }

    @Override
    public String toString() {
        return name;
    }

    public String getName() {
        return name;
    }

    public Name getNameElement() {
        return nameElement;
    }

    public CommandType getType() {
        return type;
    }

    void setType(CommandType type) {
        this.type = type;
    }

    public String getExplanation() {
        return explanation;
    }

    void setQueryRange(String contents) {
        Pattern rangePattern = Pattern.compile("\\[?([0-9]+)\\.\\.([0-9]+)\\]?");

        ArrayList<Integer> result = new ArrayList<Integer>();
        if (contents != null) {
            String[] set = contents.split(",");
            // Ok, now split it up
            for (int i = 0; i < set.length; i++) {
                String value = set[i];
                Matcher rangeMatcher = rangePattern.matcher(value);
                if (rangeMatcher.matches()) {
                    int start = Integer.parseInt(rangeMatcher.group(1));
                    int end = Integer.parseInt(rangeMatcher.group(2));
                    if (end < start) {
                        int tmp = start;
                        start = end;
                        end = tmp;
                    }
                    for (int j = start; j <= end; j++) {
                        result.add(j);
                    }
                } else {
                    try {
                        result.add(Integer.parseInt(value));
                    } catch (Exception e) {
                    }
                }
            }
        }

        queryRange = new int[result.size()];
        int cnt = 0;
        for (Integer integer : result) {
            queryRange[cnt++] = integer.intValue();
        }
    }

    public int[] getQueryRange() {
        return queryRange;
    }

    public boolean hasQueryRange() {
        return queryRange.length > 0;
    }
}
