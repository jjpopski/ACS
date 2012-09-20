package alma.acs.nc.sm.generic;

import java.util.ArrayList;
import java.util.Collection;
import java.util.EnumMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

import org.apache.commons.scxml.ErrorReporter;
import org.apache.commons.scxml.EventDispatcher;
import org.apache.commons.scxml.SCInstance;
import org.apache.commons.scxml.SCXMLExpressionException;
import org.apache.commons.scxml.TriggerEvent;
import org.apache.commons.scxml.model.CustomAction;
import org.apache.commons.scxml.model.ModelException;
import org.apache.commons.scxml.semantics.ErrorConstants;


/**
 * The user-supplied dispatcher that will be invoked by an 
 * instance of {@link AcsScxmlDispatchingAction},
 * thus bringing action handlers under user control 
 * instead of using directly the handler instantiated by the SCXML framework.
 * <p>
 * Handlers must be registered using {@link #registerActionHandler(Enum, AcsScxmlActionExecutor)}.
 * 
 * @author hsommer
 * @param <E> The SM action enumeration. 
 */
public class AcsScxmlActionDispatcher<E extends Enum<E>> 
{
	public static final String CUSTOM_ACTIONS_DOMAIN_NAME = "http://my.custom-actions.domain/CUSTOM";
	
	protected final Logger logger;
	private final Class<E> actionType;
	private final Map<E, AcsScxmlActionExecutor<E>> actionMap;

	public AcsScxmlActionDispatcher(Logger logger, Class<E> actionType) {
		this.logger = logger;
		this.actionType = actionType;
		actionMap = new EnumMap<E, AcsScxmlActionExecutor<E>>(actionType);
	}

	/**
	 * Generic action handler, dispatches to the right handler based on action type.
	 */
	public void execute(E action, final EventDispatcher evtDispatcher, final ErrorReporter errRep, final SCInstance scInstance,
			final Collection<TriggerEvent> derivedEvents) throws ModelException, SCXMLExpressionException {
		AcsScxmlActionExecutor<E> handler = getActionHandler(action);
		if (handler != null) {
			boolean handledAction = handler.execute(action, evtDispatcher, errRep, scInstance, derivedEvents);
			if (!handledAction) {
				errRep.onError(ErrorConstants.UNKNOWN_ACTION, "Handler " + handler.getClass() + " handler unexpectedly did not handle action " + action.name(), null);
			}
		}
		else {
			errRep.onError(ErrorConstants.UNKNOWN_ACTION, "No handler registered for action " + action.name(), null);
		}
	}
	
	/**
	 * An alternative to {@link #execute(Enum, EventDispatcher, ErrorReporter, SCInstance, Collection)}
	 * where the handler can be accessed and stored directly.
	 * Using this approach may bring a tiny performance advantage, but probably should be removed again..
	 */
	public AcsScxmlActionExecutor<E> getActionHandler(E action) {
		return actionMap.get(action);
	}
	
	
	/**
	 * Registers an action handler for the given SM action.
	 * Note that the same handler object can be registered for one or many actions,
	 * so that we are flexible in how we implement the handlers.
	 * @param action
	 * @param handler
	 */
	public void registerActionHandler(E action, AcsScxmlActionExecutor<E> handler) {
		actionMap.put(action, handler);
	}
	
	
	/**
	 * Checks whether all action enums have an action handler registered.
	 * <p>
	 * This method can be used to validate early at runtime that no action goes
	 * without handler, which might happen after a change in the SM model.
	 * <p>
	 * Note that this kind of completeness check could also be done at build time
	 * using an action enum with visitor pattern, see  
	 * http://raginggoblin.wordpress.com/2010/06/10/switch-over-all-values-of-a-java-enum/.
	 * However, this would require us to dispatch all actions through a single interface
	 * that has a typed method per action and whose implementation could then of course 
	 * dispatch further to various objects.
	 * For the time being we stay with the looser coupling of registering various action handler
	 * objects and validating them for completeness early at runtime (not waiting until
	 * some rare action finally occurs).
	 * 
	 * @return true if for all SM actions we have a registered handler.
	 */
	public boolean isActionMappingComplete() {
		for (E action : actionType.getEnumConstants()) {
			if (!actionMap.containsKey(action)) {
				logger.warning("No handler has been registered for SM action " + action.name() + ".");
				return false;
			}
		}
		return true;
	}
	
	/**
	 * Used by an {@link AcsScxmlDispatchingAction} object to convert its action name to the corresponding enum value.
	 * (This type information cannot be passed directly because Apache SCXML creates the action with an empty constructor.)
	 */
	public Class<E> getActionType() {
		return actionType;
	}
	
	
	/**
	 * Wraps the registered action handlers in a form suitable for the SCXML framework.
	 * The framework will instantiate DispatchingScxmlAction for each action,
	 * which will then call back to {@link #execute(Enum, EventDispatcher, ErrorReporter, SCInstance, Collection)}. 
	 * @return
	 */
	protected List<CustomAction> getScxmlActionMap() {
		List<CustomAction> ret = new ArrayList<CustomAction>();
		
		for (E action : actionMap.keySet()) {
			ret.add(new CustomAction(CUSTOM_ACTIONS_DOMAIN_NAME, action.name(), AcsScxmlDispatchingAction.class));
		}
		return ret;
	}

}