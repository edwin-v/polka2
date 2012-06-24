#include "HistoryWindow.h"
#include "Settings.h"
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <glibmm/i18n.h>
#include <gtkmm/activatable.h>

#include <iostream>
using namespace std;

namespace Polka {

HistoryWindow::HistoryWindow()
	: m_UndoButton( Gtk::Stock::UNDO ), m_RedoButton( Gtk::Stock::REDO ), m_pHistory(0)
{
	set_title(_("History"));
	set_type_hint( Gdk::WINDOW_TYPE_HINT_UTILITY );
	set_border_width(5);
	set_skip_taskbar_hint();

	add( m_MainBox );

	Gtk::ScrolledWindow *swin = manage(new Gtk::ScrolledWindow);
	swin->set_shadow_type( Gtk::SHADOW_IN );
	swin->add(m_ListView);
	swin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	m_MainBox.pack_start( *swin );
	m_MainBox.pack_start( m_ButtonBox, Gtk::PACK_SHRINK );
	m_ButtonBox.pack_start( m_UndoButton, Gtk::PACK_SHRINK );
	m_ButtonBox.pack_start( m_RedoButton, Gtk::PACK_SHRINK );
	//m_UndoButton.set_relief( Gtk::RELIEF_NONE );
	//m_RedoButton.set_relief( Gtk::RELIEF_NONE );

	m_refListModel = Gtk::ListStore::create(m_Columns);
	m_ListView.set_model(m_refListModel);
	m_ListView.append_column( "I", m_Columns.m_ColType );
	m_ListView.append_column( "I", m_Columns.m_ColIcon );
	m_ListView.append_column( "T", m_Columns.m_ColName );
	m_ListView.set_sensitive(false);

	m_ListView.set_headers_visible(false);

	m_itLastUndo = m_refListModel->append();
	Gtk::TreeModel::Row row = *m_itLastUndo;
	row[m_Columns.m_ColName] = _("Start of history");
	row[m_Columns.m_pAction] = 0;

	m_itFirstRedo = m_refListModel->children().end();
	updateButtonSensitivity();

	// connect signals
	m_UndoButton.signal_clicked().connect( sigc::mem_fun(*this, &HistoryWindow::onUndoClick ) );
	m_RedoButton.signal_clicked().connect( sigc::mem_fun(*this, &HistoryWindow::onRedoClick ) );
	m_ConnSelection = m_ListView.get_selection()->signal_changed().connect( sigc::mem_fun(*this, &HistoryWindow::onSelect) );
	m_ConnSelection.block();

	m_refUndoIcon = render_icon_pixbuf( Gtk::Stock::UNDO, Gtk::ICON_SIZE_SMALL_TOOLBAR );
	m_refRedoIcon = render_icon_pixbuf( Gtk::Stock::REDO, Gtk::ICON_SIZE_SMALL_TOOLBAR );

	show_all_children();
}

HistoryWindow::~HistoryWindow()
{
}

void HistoryWindow::on_show()
{
	Settings::get().getWindowGeometry( "", "HistoryWindowGeometry", *this, -1, -1, 200, 350 );
	Gtk::Window::on_show();
}

void HistoryWindow::on_hide()
{
	Settings::get().setWindowGeometry( "", "HistoryWindowGeometry", *this );
	Gtk::Window::on_hide();
}

void HistoryWindow::setUndoHistory( UndoHistory *hist )
{
	m_ConnChange.disconnect();
	if( m_pHistory ) {
		m_ConnSelection.block();
		m_refUndoAction->set_sensitive( false );
		m_refRedoAction->set_sensitive( false );
		m_UndoButton.set_sensitive( false );
		m_RedoButton.set_sensitive( false );
		clearUndoRows();
		clearRedoRows();
		m_ListView.set_sensitive(false);
	}
	m_pHistory = hist;

	if( m_pHistory ) {
		// connect signals
		m_ConnChange = m_pHistory->signalHistoryChanged().connect( sigc::mem_fun(*this, &HistoryWindow::historyChanged ) );
		m_ListView.set_sensitive(true);
		m_ConnSelection.block(false);
	}
	
	
}

void HistoryWindow::setActions( const Glib::RefPtr<Gtk::Action>& undoAction, const Glib::RefPtr<Gtk::Action>& redoAction )
{
	m_refUndoAction = undoAction;
	m_refRedoAction = redoAction;
	m_refUndoAction->set_sensitive( m_UndoButton.get_sensitive() );
	m_refRedoAction->set_sensitive( m_RedoButton.get_sensitive() );
	//m_UndoButton.set_related_action( undoAction );
	//m_RedoButton.set_related_action( redoAction );
}

void HistoryWindow::onUndoClick()
{
	if( m_pHistory ) m_pHistory->undo();
}

void HistoryWindow::onRedoClick()
{
	if( m_pHistory ) m_pHistory->redo();
}

void HistoryWindow::onSelect()
{
	Gtk::TreeModel::iterator selected = m_ListView.get_selection()->get_selected();

	if( m_refRedoIcon == (*selected)[m_Columns.m_ColType] ) {
		while( m_itLastUndo != selected ) m_pHistory->redo();
	} else {
		while( m_itLastUndo != selected ) m_pHistory->undo();
	}
}

void HistoryWindow::historyChanged( UndoHistory::ChangeType type )
{
	switch(type) {
		case UndoHistory::CHANGE_ALLUNDO:
			clearUndoRows();
			break;
		case UndoHistory::CHANGE_ALLREDO:
			clearRedoRows();
			break;
		case UndoHistory::CHANGE_ADDUNDO:
			addLastUndoRow();
			break;
		case UndoHistory::CHANGE_UNDOACTION:
			undoAction();
			break;
		case UndoHistory::CHANGE_REDOACTION:
			redoAction();
			break;
		default:
			break;
	}
	updateButtonSensitivity();
}

void HistoryWindow::clearRedoRows()
{
	while( m_itFirstRedo != m_refListModel->children().end() ) {
		Gtk::TreeModel::iterator del = m_itFirstRedo;
		m_itFirstRedo++;
		m_refListModel->erase(del);
	}
}

void HistoryWindow::clearUndoRows()
{
	// block signal because selection must be changed
	bool block = m_ConnSelection.blocked();
	m_ConnSelection.block();
	m_ListView.get_selection()->select( m_refListModel->children().begin() );
	// remove entries
	while( m_itLastUndo != m_refListModel->children().begin() ) {
		Gtk::TreeModel::iterator del = m_itLastUndo;
		std::cout << "delete " << (*del)[m_Columns.m_ColName] << std::endl;
		m_itLastUndo--;
		m_refListModel->erase(del);
	}
	// turn on signal if it was on
	if( !block ) m_ConnSelection.block(false);
}

void HistoryWindow::addLastUndoRow()
{
	// assume undo row exists
	const UndoAction& action = m_pHistory->lastUndoAction();

	cout << "Undo added -- " << action.name().raw() << 
         " UndoID: " << action.undoId() << 
         " RedoID: " << action.redoId() << endl;
	
	m_itLastUndo = m_refListModel->append();
	Gtk::TreeModel::Row row = *m_itLastUndo;
	row[m_Columns.m_ColType] = m_refUndoIcon;
	row[m_Columns.m_ColIcon] = action.userActionIcon();
	row[m_Columns.m_ColName] = action.userActionName();
	row[m_Columns.m_pAction] = &action;
	
	m_UndoButton.set_sensitive();
}

void HistoryWindow::undoAction()
{
	m_itFirstRedo = m_itLastUndo;
	--m_itLastUndo;
	(*m_itFirstRedo)[m_Columns.m_ColType] = m_refRedoIcon;
}

void HistoryWindow::redoAction()
{
	m_itLastUndo = m_itFirstRedo;
	++m_itFirstRedo;
	(*m_itLastUndo)[m_Columns.m_ColType] = m_refUndoIcon;
}

void HistoryWindow::updateButtonSensitivity()
{
	m_ListView.get_selection()->select( m_itLastUndo );
	m_UndoButton.set_sensitive( (*m_itLastUndo)[m_Columns.m_pAction] != 0 );
	m_RedoButton.set_sensitive( m_itFirstRedo != m_refListModel->children().end() );
	if( m_refUndoAction ) m_refUndoAction->set_sensitive( m_UndoButton.get_sensitive() );
	if( m_refRedoAction ) m_refRedoAction->set_sensitive( m_RedoButton.get_sensitive() );
}

} // namespace ...

