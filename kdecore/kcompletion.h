/* This file is part of the KDE libraries
    Copyright (C) 1999,2000 Carsten Pfeiffer <pfeiffer@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef KCOMPLETION_H
#define KCOMPLETION_H

#include <qmap.h>
#include <qlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kglobalsettings.h>

#include "kcompletion_private.h"

class KCompletionPrivate;
class QPopupMenu;

/**
 * This class let's you easily use "auto-completion", "manual-completion" or
 * "shell completion" on QString objects. A common use is completing filenames
 * or URLs (see @ref KURLCompletion).
 * But URL-completion is not all, everything should be completable. The user
 * should be able to complete email-addresses, telephone-numbers, commands,
 * SQL queries, ...
 * Everytime your program knows what the user can type into an edit-field, you
 * should offer completion. With KCompletion, this is very easy, and if you are
 * using a LineEdit-widget (@ref KLineEdit), it is even more easy.
 * Basically, you tell a KCompletion-object what strings should be completable
 * and then, whenever the user types something, you call @ref makeCompletion().
 * KLineEdit and (an editable) KComboBox even do this automatically for you.
 *
 * KCompletion offers the completed string via the signal @ref match() and
 * all matching strings via the method @ref allMatches().
 *
 * Notice: auto-completion, shell completion and manual completion work
 *         slightly differently:
 *
 * @li auto-completion always returns a complete item as match.
 *     When more than one matching items are available, it will deliver just
 *     the first (depending on sorting order) item. Iterating thru them is
 *     possible via @ref nextMatch() and @ref previousMatch().
 *
 * @li manual completion works the same way as auto-completion, the
 *     subtle difference is, that it isn't invoked automatically while the user
 *     is typing, but only when the user presses a special key. The difference
 *     of manual and auto-completion is therefore only visible in UI classes,
 *     KCompletion needs to know about whether to deliver partial matches
 *     (shell completion) or whole matches (auto/manual completion), therefore
 *     @ref KGlobalSettings::CompletionMan and
 *     @ref KGlobalSettings::CompletionAuto have the exact same effect in
 *     KCompletion.
 *
 * @li shell completion works like how shells complete filenames.
 *     When multiple matches are available, the longest possible string of all
 *     matches is returned (i.e. only a partial item).
 *     Iterating thru all matching items (complete, not partial) is possible
 *     via @ref nextMatch() and @ref previousMatch().
 *
 * You don't have to worry much about that though, KCompletion handles
 * that for you, according to the setting @ref setCompletionMode().
 * The default setting is globally configured by the user and read
 * from @ref KGlobalSettings::completionMode().
 *
 * A short example:
 * <pre>
 * KCompletion completion;
 * completion.setSorted( true );
 * completion.addItem( "pfeiffer@kde.org" );
 * completion.addItem( "coolo@kde.org" );
 * completion.addItem( "carpdjih@sp.zrz.tu-berlin.de" );
 * completion.addItem( "carp@cs.tu-berlin.de" );
 *
 * debug( completion.makeCompletion( "ca" ).latin1() );
 * </pre>
 * In shell-completion-mode, this will be "carp"; in auto-completion-
 * mode it will return "carp@cs.tu-berlin.de", as that is alphabetically
 * smaller.
 * If setSorted was set to false (default), "carpdjih@sp.zrz.tu-berlin.de"
 * would be completed in auto-completion-mode, as that was inserted before
 * "carp@cs.tu-berlin.de".
 *
 * You can dynamically update the completable items by removing and adding them
 * whenever you want.
 * For advanced usage, you could even use multiple KCompletion objects (e.g.
 * imagine an editor like kwrite with multiple open files. You could store
 * items of every file in a different KCompletion-object, so that you know (and
 * tell the user) where a completion comes from.
 *
 * Note: KCompletion does not work with strings that contain 0x0 characters
 *       (unicode nul), as this is used internally as a delimiter.
 *
 * You may inherit from KCompletion and override @ref makeCompletion() in
 * special cases (like reading directories/urls and then supplying the
 * contents to KCompletion, as KURLCompletion does), but generally, this is
 * not necessary.
 *
 *
 * @short A generic class for completing QStrings
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 * @version $Id$
 */
class KCompletion : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor, nothing special here :)
     */
    KCompletion();

    // FIXME: copy constructor, assignment constructor...

    /**
     * Destructor, nothing special here, either.
     */
    virtual ~KCompletion();

    /**
     * Attempts to find an item in the list of available completions,
     * that begins with string. Will either return the first (if more than one
     * match) matching item or QString::null, if no match was found. In the
     * latter case, a beep will be issued, depending on @ref isSoundsEnabled().
     * If a match was found, it will also be emitted via the signal
     * @ref match().
     *
     * If this is called twice or more often with the same string while no
     * items were added or removed in the meantime, all available completions
     * will be emitted via the signal @ref matches().
     * This happens only in shell-completion-mode.
     *
     * @returns the matching item, or QString::null if there is no matching
     * item.
     * @see #slotMakeCompletion
     */
    virtual QString makeCompletion( const QString& string );

    /**
     * @returns the next item from the matching-items-list
     * When reaching the beginning, the list is rotated, so it will return
     * the last match. When there is no match, QString::null is returned and
     * a beep will be issued, depending on @ref isSoundsEnabled().
     * @see #slotPreviousMatch
     */
    QString previousMatch();

    /**
     * @returns the previous item from the matching-items-list
     * When reaching the last item, the list is rotated, so it will return
     * the first match. When there is no match, QString::null is returned and
     * a beep will be issued, depending on @ref isSoundsEnabled().
     * @see #slotNextMatch
     */
    QString nextMatch();

    /**
     * @returns the last match. Might be useful if you need to check whether
     * a completion is different from the last one.
     * QString::null is returned when there is no last match.
     */
    virtual const QString& lastMatch() const { return myLastMatch; }


    /**
     * @returns a list of all items inserted into KCompletion. This is useful
     * if you need to save the state of a KCompletion object and restore it
     * later.
     * @see #setItems
     */
    QStringList items() const;


    /**
     * Sets the completion mode to Auto/Manual (see @ref KCompletion
     * documentation), Shell or None.
     * If you don't set the mode explicitly, the global default value
     * KGlobalSettings::completionMode() is used.
     * @ref KGlobalSettings::CompletionNone disables completion.
     * @see #completionMode
     * @see #KGlobalSettings::completionMode
     */
    void setCompletionMode( KGlobalSettings::Completion mode );

    /**
     * @returns the current completion mode.
     * May be different from @ref KGlobalSettings::completionMode(), if you
     * explicitly called @ref setCompletionMode().
     * @see #setCompletionMode
     */
    KGlobalSettings::Completion completionMode() const { return myCompletionMode; }

    /**
     * Setting this to true makes us go into sorted mode (doh).
     * Completion will then always return the alphabetically first match.
     * If set to false, the order is the same as the items were inserted.
     * Note: this only affects new inserted items, already existing items will
     * stay in the current order. So you probably want to call setSorted( true )
     * before inserting items, when you want everything sorted.
     * Default is false, not sorted.
     * @see #isSorted
     */
    void setSorted( bool enable ) { mySorting = enable; }

    /**
     * Setting this to true makes KCompletion behave case insensitively.
     * E.g. makeCompletion( "CA" ); might return "carp@cs.tu-berlin.de".
     * Default is false (case sensitive).
     * @see #ignoreCase
     */
    //  void setIgnoreCase( bool ignoreCase ) { myIgnoreCase = ignoreCase; }

    /**
     * @returns whether KCompletion acts case insensitively or not.
     * Default is false (case sensitive).
     * @see #setIgnoreCase
     */
    //  bool ignoreCase() const { return myIgnoreCase; }

    /**
     * @returns true if the completion-items are alphabetically sorted and
     * false if the order of insertion is used.
     * @see #setSorted
     */
    bool isSorted() const { return mySorting; }

    /**
     * @returns a list of all matching items. Might take some time, when you
     * have LOTS of items.
     */
    QStringList allMatches() { return findAllCompletions( myLastString ); }

    /**
     * Enables playing a sound when
     * @li @ref makeCompletion() can't find a match
     * @li there is a partial completion
     * @li @ref nextMatch() or @ref previousMatch() hit the last possible
     *     match -> rotation
     *
     * @see #disableSounds
     * @see #isSoundEnabled
     */
    void enableSounds() { myBeep = true; }

    /**
     * Disables playing a sound when
     * @li @ref makeCompletion() can't find a match
     * @li there is a partial completion
     *
     * Sounds are only played in shell-completion mode. Default is enabled
     * @see #enableSounds
     * @see #isSoundEnabled
     */
    void disableSounds() { myBeep = false; }

    /**
     * Tells you whether KCompletion will issue beeps (@ref KApplication::beep())
     * Beeps only in manual-completion mode
     * Default is enabled
     * @see #enableSounds
     * @see #disableSounds
     */
    bool isSoundsEnabled() const { return myBeep; }


public slots:
    /**
     * Attempts to complete "string" and emits the completion via @ref match().
     * Same as @ref makeCompletion() (just as a slot).
     * @see #makeCompletion
     */
    void slotMakeCompletion( const QString& string ) {
	(void) makeCompletion( string );
    }

    /**
     * Searches the previous matching item and emits it via @ref match()
     * Same as @ref previousMatch() (just as a slot).
     * @see #previousMatch
     */
    void slotPreviousMatch() {
	(void) previousMatch();
    }

    /**
     * Searches the next matching item and emits it via @ref match()
     * Same as @ref nextMatch() (just as a slot).
     * @see #nextMatch
     */
    void slotNextMatch() {
	(void) nextMatch();
    }

    /**
     * @returns true when more than one match is found
     * @see #multipleMatches
     */
    bool hasMultipleMatches() const { return myHasMultipleMatches; }

    /**
     * Sets the list of items available for completion. Removes all previous
     * items.
     * @see #items
     */
    void setItems( const QStringList& );

    /**
     * Adds an item to the list of available completions.
     * Resets the current item-state (@ref previousMatch() and @ref nextMatch()
     * won't work anymore).
     */
    void addItem( const QString& );

    /**
     * Removes an item from the list of available completions.
     * Resets the current item-state (@ref previousMatch() and @ref nextMatch()
     * won't work anymore).
     */
    void removeItem( const QString& );

    /**
     * Clears the list of inserted items.
     */
    void clear();


signals:
    /**
     * The matching item. Will be emitted by @ref makeCompletion(),
     * @ref previousMatch() or @ref nextMatch(). May be QString::null if there is
     * no matching item.
     */
    void match( const QString& );

    /**
     * All matching items. Will be emitted by @ref makeCompletion() in shell-
     * completion-mode, when the same string is passed to makeCompletion twice
     * or more often.
     */
    void matches( const QStringList& );

    /**
     * This signal is emitted, when calling @ref makeCompletion() and more than
     * one matching item is found.
     * @see #hasMultipleMatches
     */
    void multipleMatches();

protected:
    /**
     * This method is called after a completion is found and before the
     * matching string is emitted. You can override this method to modify the
     * string that will be emitted.
     * This is necessary e.g. in @ref KURLCompletion, where files with spaces
     * in their names are shown escaped ("filename\ with\ spaces"), but stored
     * unescaped inside KCompletion.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @see #postProcessMatches
     */
    virtual void postProcessMatch( QString * /*match*/ ) {}

    /**
     * This method is called before a list of all available completions is
     * emitted via @ref matches. You can override this method to modify the
     * list which that will be emitted.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @see #postProcessMatch
     */
    virtual void postProcessMatches( QStringList * /*matches*/ ) {}

private:
    void 		addItemInternal( const QString& );
    QString 		findCompletion( const QString& string );
    const QStringList& 	findAllCompletions( const QString& );
    void 		extractStringsFromNode( const KCompTreeNode *,
						const QString& beginning,
						QStringList *matches ) const;

    enum 		BeepMode { NoMatch, PartialMatch, Rotation };
    void 		doBeep( BeepMode );

    QStringList         myMatches;
    KGlobalSettings::Completion myCompletionMode;

    QString             myLastString;
    QString 		myLastMatch;
    QString 		myCurrentMatch;
    KCompTreeNode *     myTreeRoot;
    QStringList 	myRotations;
    bool                mySorting;
    bool                myBeep;
    bool 		myBackwards;
    bool 		myIgnoreCase;
    bool 		myHasMultipleMatches;
    int 		myItemIndex; // FIXME
    uint 		myRotationIndex;

    KCompletionPrivate *d;
};


/**
 * An abstract base class for adding completion feature
 * into widgets.
 *
 * This is a convienence class that tries to provide
 * the common functions needed to add support for
 * completion into widgets.  Refer to @ref KLineEdit
 * or @ref KComboBox to see how to such support can be
 * added using this base class.
 *
 * NOTE: Do not forget to provide an implementation for
 * the protected pure virtual method @ref connectSignals()
 * if you do not want the class to be abstract.
 *
 * @short An abstract class for using KCompletion in widgets
 * @author Dawit Alemayehu <adawit@kde.org>
 */
class KCompletionBase
{

public:

	/**
	* Constants that represent the items whose short-cut
	* key-binding is programmable.  The default key-bindings
	* for these items are defined in @ref KStdAccel.
	*/	
	enum KeyBindingType {
		TextCompletion,
		PrevCompletionMatch,
		NextCompletionMatch,
		RotateUp,
		RotateDown
	};
	
	// Map for the key binding types mentioned above.
	typedef QMap<KeyBindingType, int> KeyBindingMap;

    /**
    * Default constructor.
    */
    KCompletionBase();

    /**
    * Basic destructor.
    */
    virtual ~KCompletionBase();

    /**
    * Returns a pointer to the current completion object.
    *
    * If the object does not exisit, it is automatically
    * created.  Note that the completion object created
    * here is used by default to handle the signals
    * internally.  It is also deleted when this object's
    * destructor is invoked.  If you do not want these
    * default settings, use @ref setAutoDeleteCompletionObject
    * and @ref setHandleSignals to change the behavior.
    * Alternatively, you can set the boolean parameter to
    * false to disable the automatic handling of the signals
    * by this object.  Note that the boolean argument will be
    * ignored if there already exists a completion object since
    * no new object needs to be created. You need to use either
    * @ref setHandleSignals or @ref setCompletionObject for
    * such cases depending on your requirement.
    *
    * @param hsig if true, handles signals internally.
    * @return a pointer the completion object.
    */
    KCompletion* completionObject( bool hsig = true );

    /**
    * Sets up the completion object to be used.
    *
    * This method assigns the completion object and sets it
    * up to automatically handle the completion and rotation
    * signals internally.  You should use this function if
    * you want to share one completion object among you widgets
    * or need to use a customized completion object.
    *
    * The object assigned through this method is not deleted
    * when this object's destructor is invoked unless you
    * explicitly call @ref setAutoDeleteCompletionObject after
    * calling this method.  Also if you do not want the signals
    * to be handled by an internal implementation, be sure to
    * set the bool argument to false.
    *
    * @param compObj a @ref KCompletion or a derived child object.
    * @param hsig if true, handles signals internally.
    */
    virtual void setCompletionObject( KCompletion* compObj, bool hsig = true );

    /**
    * Enables this object to handle completion signals
    * internally.
    *
    * This function simply assigns the boolean value that
    * indicates whether it should handle signals or not.
    * All child objects must provide an implementation for
    * @ref connectSignals() which this method calls first.
    *
    * @param complete if true, handle completion & roation internally.
    */
    void setHandleSignals( bool handle );

    /**
    * Returns true if the completion object is deleted
    * upon this widget's destruction.
    *
    * See @ref setCompeltionObject and @ref enableCompletion
    * for details.
    *
    * @return true if the completion object
    */
    bool isCompletionObjectAutoDeleted() const { return m_bAutoDelCompObj; }

    /**
    * Sets the completion object for deletion upon this
    * widget's destruction.
    *
    * If the argument is set to true, the completion object
    * is deleted when this widget's destructor is called.
    *
    * @param autoDelete if true, delete completion object on destruction.
    */
    void setAutoDeleteCompletionObject( bool autoDelete ) { m_bAutoDelCompObj = autoDelete; }

    /**
    * Disables (temporarily) this widget's ability to emit
    * the rotation and completion signals.
    *
    * Invoking this function will cause the completion &
    * rotation signals not to be emitted.  Note that this
    * also disbales any internal handling of these signals.
    * However, unlike @ref setCompletionObject object,
    * disabling the emition of the signals through this
    * method does not delete the comlpetion object.
    */
    void disableSignals() { m_bEmitSignals = false; }

    /**
    * Enables the widget's ability to emit completion signals.
    *
    * Note that there is no need to invoke this function by
    * default.  When a completion object is created through
    * completionObject() or setCompletionObject(), these
    * signals are automatically actiavted.  Only call this
    * functions if you disbaled them manually.
    */
    void enableSignals()  { m_bEmitSignals = true; }

    /**
    * Returns true if the object handles the signals
    *
    * @return true if this signals are handled internally.
    */
    bool handleSignals() const { return m_bHandleSignals; }

    /**
    * Returns true if the object emits the signals
    *
    * @return true if signals are emitted
    */
    bool emitSignals() const { return m_bEmitSignals; }

    /**
    * Sets the type of completion to be used.
    *
    * The completion modes supported are those defined in
    * @ref KGlobalSettings.  See below.
    *
    * @param mode Completion type:
    *   @li CompletionNone:  Disables completion feature.
    *   @li CompletionAuto:  Attempts to find a match &
    *                        fills-in the remaining text.
    *   @li CompletionMan:   Acts the same as the above
    *                        except the action has to be
    *                        manually triggered through
    *                        pre-defined completion key.
    *   @li CompletionShell: Mimics the completion feature
    *                        found in typcial *nix shell
    *                        enviornments.
    */
    virtual void setCompletionMode( KGlobalSettings::Completion mode );

    /**
    * Retrieves the current completion mode.
    *
    * The return values are of type @ref KGlobalSettings::Completion.
    * See @ref setCompletionMode for details.
    *
    * @return the completion mode.
    */
    KGlobalSettings::Completion completionMode() const { return m_iCompletionMode; }

    /**
    * Sets the key-binding to be used for manual text
    * completion, text rotation in a history list as
    * well as a completion list.
    *
	*
    * When the keys set by this function are pressed, a
    * signal defined by the inherting widget will be activated.
    * If the default value or 0 is specified by the second
    * parameter, then the key-binding as defined in the global
    * setting should be used.  This method returns false value
    * for @p key is negative or the supplied key-binding conflicts
    * with the ones set for one of the other features.
    *
    * NOTE: To use a modifier key (Shift, Ctrl, Alt) as part of
    * the key-binding simply simply @p sum up the values of the
    * modifier and the actual key.  For example, to use CTRL+E as
    * a key binding for one of the items, you would simply supply
    * @p "Qt::CtrlButton + Qt::Key_E" as the second argument to this
    * function.
    *
    * @param item the feature whose key-binding needs to be set:
    *
	*		@li TextCompletion			the manual completion
	*									key-binding.
	*		@li PrevCompletionMatch		the previous match key
	*									for mutiple completion.
	*		@li	NextCompletionMatch		the next match key for
	*									for multiple completion.
	*		@li	RotateUp				the key for rotating up
	*									in a given list.
	*		@li	RotateDown				the key for rotating down
	*									in a given list.
    *
    * @param key key-binding used to rotate down in a list.
    *
    * @return  true if key-binding can successfully be set.
    */
    bool setKeyBinding( KeyBindingType /*item*/ , int key = 0 );

    /**
    * Returns the key-binding used for the specified item.
    *
    * This methods returns the key-binding used to activate
    * the feature feature given by @p item.  If the binding
    * contains modifier key(s), the SUM of the modifier key
    * and the actual key code are returned.
    *
    * @return the key-binding used for the feature given by @p item.
    */
    int getKeyBinding( KeyBindingType item ) const { return m_keyMap[ item ]; }

    /**
    * Sets this object to use global values for key-bindings.
    *
    * This method changes the values of the key bindings for
    * rotation and completion features to the default values
    * provided in KGlobalSettings.
    *
    * NOTE: By default inheriting widgets should uses the
    * global key-bindings so that there will be no need to
    * call this method.
    */
    void useGlobalKeyBindings();

    /**
    * Makes the completion mode changer visible in the context
    * menu.
    *
    * Note that the mode changer item is a sub menu, that allows
    * the user to select from one of the standard completion modes
    * described at @ref setCompletionMode. Additionally, if the user
    * changes the completion mode to something other than the global
    * setting, a "Default" entry is added at the bottom to allow the
    * user to revert his/her changes back to the global setting.
    */
    void showModeChanger()  { m_bShowModeChanger = true; }

    /**
    * Hides the completion mode changer in the context menu.
    */
    void hideModeChanger() { m_bShowModeChanger = false; }

    /**
    * Returns true if the mode changer item is visible in
    * the context menu.
    *
    * @return @p true if the mode changer is visible in context menu.
    */
    bool isModeChangerVisible() const { return m_bShowModeChanger; }

protected:

    /**
    * A pure virtual function.
    *
    * This function must provide an implementation for
    * how signals are connected when they are handled
    * internally.  Since this is implementation dependent
    * it is left upto each inheriting object to decide.
    */
    virtual void connectSignals( bool handle ) const = 0;

    /**
    * Inserts completion mode changers items.
    *
    * This method is implemented here as a matter of
    * convience and for the sake of consistency in the
    * appearance of the completion mode changer items.
    * It initially inerts four of the standard menu items
    * labeled: "None", "Manual", "Automatic" and "Semi-Automatic".
    * and puts a check mark beside the mode that is currently
    * active.  When the user changes this default mode, another
    * item labeled "Default" is inserted so that the user can
    * quickly revert back to the default mode.
    *
    * To use this method simply invoke this function from your
    * implementation of the slot supplied to @ref insertCompletionMenu.
    * See @ref KLineEdit::showCompletionItems or @ref KComboBox::showCompletionItems
    * for an example of implementation.
    *
    * Note that when this "Default" item, the clicked signal emitted
    * has its integer argument set to 0.  Thus, you need to make sure
    * that you replace this value with the one returned by
    * @ref KGlobalSettings::completionMode().
    *
    * @param receiver object that receives the activation of completion items.
    * @param member method invoked when completion items are clicked.
    */
    void insertCompletionItems( QObject* receiver, const char* member );

    /**
    * Adds a completion menu item to the given popup menu.
    *
    * This function adds/removes a completion menu item at the
    * specified index position in the popup menu.  If index
    * is a negative value, the menu item is inserted at the end
    * of the parent popup menu.  Since this function manages the
    * addition and removal of the completion menu item by itself,
    * it is intended/designed to be invoked just before you display
    * your popup menu.
    *
    * See @ref KLineEdit::aboutToShow or @ref KComboBox::aboutToShow
    * for an example implementation.
    *
    * @param receiver object that receives the activation of completion items.
    * @param member method invoked when completion sub-menu is about to be shown.
    * @param parent if not NULL, add the completion option as its sub-menu.
    * @param index if non-negative, add the compleiton option at the specified position.
    */
    void insertCompletionMenu( QObject* receiver, const char* member, QPopupMenu* parent, int index = -1 );

    /**
    * Returns true if completion menu item has been
    * inserted into the popup menu.
    *
    * @return true if completion menu is inserted into popup menu.
    */
    bool hasBeenInserted() { return m_iCompletionID != -1; }

protected:

	/**
	 * Returns an instance of the completion object.
	 *
	 * This method is only different from @ref completionObject
	 * in that it does not create a new KCompletion object if
	 * the internal pointer is null.  Use this method to get the
	 * pointer to a completion object when inheriting from this
	 * widget so that you will not inadvertantly create it!!
	 *	
	 * @ref an instance of the completion object.
	 */
     KCompletion* compObj() const { return m_pCompObj; }

    /**
    * Returns a key-binding maps
    *
    * This method is the same as @ref getKeyBinding except it
    * returns the whole keymap containing the key-bindings.
    *
    * @return the key-binding used for the feature given by @p item.
    */
	KeyBindingMap getKeyBindings() const { return m_keyMap; }

private:
    // Stores the completion menu id.  Used to
    // determine whether the completion menu
    // has already been inserted or not!!
    int m_iCompletionID;

    // Flag that determined whether the completion object
    // should be deleted when this object is destroyed.
    bool m_bAutoDelCompObj;
    // Determines whether this widget handles completion signals
    // internally or not
    bool m_bHandleSignals;
    // Determines whether this widget fires rotation signals
    bool m_bEmitSignals;
    // Determines if completion mode changer
    // should be visible.
    bool m_bShowModeChanger;

    // Stores the completion mode locally.
    KGlobalSettings::Completion m_iCompletionMode;
    // Pointer to Completion object.
    KCompletion* m_pCompObj;
    // Pointer to a completion popupmenu.
    QPopupMenu* m_pCompletionMenu;
    // Pointer for future binary compatabilty.
    KCompletionPrivate *d;
    // Keybindings
	KeyBindingMap m_keyMap;
};

#endif // KCOMPLETION_H
