using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using System.Windows.Forms;

namespace CS_Browser
{
	public partial class BrowserForm : Form
	{
		private List<IBrowserListener> browserListeners;

		private string currentFile;
		private bool isDirty;
		private string m_BaseTitle;

		internal BrowserForm()
		{
			InitializeComponent();

			this.browserListeners = new List<IBrowserListener>();
			this.currentFile = null;
			this.m_BaseTitle = this.Text;
			this.SetDirtyFlag( true );
			this.FormClosing += this.BrowserForm_FormClosing;
		}

		

		




		#region Private Save Functions

		private enum SaveOperationStatus
		{
			CANCELED,
			OK
		}

		/// <summary>
		/// Saves the current file only if the dirty flag is true. It will also optionally ask the user if they
		/// want to save their work. If askUser is set to false and the current file is marked as dirty, this will
		/// save. If askUser is set to true, and the user presses cancel, the file will not be saved, and this function
		/// will return SaveOperationStatus.CANCELED. If the user presses No, the file will not be saved, but this
		/// function will return SaveOperationStatus.OK
		/// </summary>
		/// 
		/// <param name="askUser">
		/// if set to true, this function will also spawn a dialog to ask if the user wants to save their work
		/// </param>
		/// 
		/// <returns> OK if the save operation was successful, CANCELED if the user canceled the save process </returns>
		private SaveOperationStatus SaveIfDirty( bool askUser = true )
		{
			SaveOperationStatus status = SaveOperationStatus.OK;

			if( this.isDirty )
			{
				DialogResult buttonPressed = DialogResult.Yes;
				if( askUser )
				{
					// The other window needs to act like the message box is a modal dialog
					buttonPressed = MessageBox.Show( this, "Do you want to save the current level?", "Save", MessageBoxButtons.YesNoCancel, MessageBoxIcon.None, MessageBoxDefaultButton.Button1 );
				}

				if( buttonPressed == DialogResult.Cancel )
				{
					status = SaveOperationStatus.CANCELED;
				}
				else if( buttonPressed == DialogResult.Yes )
				{
					status = this.SaveLevel();
				}
			}

			return status;
		}

		private SaveOperationStatus SaveLevel( bool saveAs = false )
		{
			if( this.currentFile == null || saveAs )
			{
				SaveFileDialog saveDialog = new SaveFileDialog();
				saveDialog.AddExtension = true;
				saveDialog.DefaultExt = ".dcl";
				saveDialog.CheckFileExists = false;
				saveDialog.CheckPathExists = true;
				saveDialog.Filter = "Uncooked Level Files (*.dcl)|*.dcl|All files (*.*)|*.*";
				saveDialog.FilterIndex = 1;
				saveDialog.OverwritePrompt = true;
				saveDialog.RestoreDirectory = true;
				saveDialog.ShowHelp = false;
				saveDialog.SupportMultiDottedExtensions = true;
				saveDialog.Title = "Save Level";
				saveDialog.ValidateNames = true;

				DialogResult result = saveDialog.ShowDialog( this );

				if( DialogResult.OK == result )
				{
					this.currentFile = saveDialog.FileName;
				}
				else
				{
					return SaveOperationStatus.CANCELED;
				}
			}

			foreach( IBrowserListener listener in this.browserListeners )
			{
				listener.OnSaveLevel( this.currentFile );
			}

			return SaveOperationStatus.OK;
		}

		#endregion


		private SaveOperationStatus Exit()
		{
			if( SaveOperationStatus.CANCELED == this.SaveIfDirty() ) return SaveOperationStatus.CANCELED;

			foreach( IBrowserListener listener in this.browserListeners )
			{
				listener.OnExit();
			}

			return SaveOperationStatus.OK;
		}

		private void DeselectAll()
		{
			this.listActors.ClearSelected();
			this.listLoadedModels.ClearSelected();
		}



		#region Browser Window Function Hooks

		private void BrowserForm_FormClosing( object sender, FormClosingEventArgs e )
		{
			if( SaveOperationStatus.CANCELED == this.Exit() )
			{
				e.Cancel = true;
			}
		}

		private void BrowserForm_Load( object sender, EventArgs e )
		{
			// Do nothing
		}

		protected override bool ProcessCmdKey( ref Message msg, Keys keyData )
		{
			if( keyData == Keys.Escape )
			{
				this.DeselectAll();
				return true;
			}

			return base.ProcessCmdKey( ref msg, keyData );
		}

		private void newToolStripMenuItem_Click( object sender, EventArgs e )
		{
			this.listActors.ClearSelected();
			this.listLoadedModels.ClearSelected();

			if( SaveOperationStatus.CANCELED == this.SaveIfDirty() ) return;

			this.currentFile = null;

			foreach ( IBrowserListener listener in this.browserListeners )
			{
				listener.OnNewLevel();
			}
		}

		private void openLevelToolStripMenuItem_Click( object sender, EventArgs e )
		{
			this.SaveIfDirty();

			OpenFileDialog openDialog = new OpenFileDialog();
			openDialog.RestoreDirectory = true;
			openDialog.Filter = "Uncooked Level Files (*.dcl)|*.dcl|All files (*.*)|*.*";
			openDialog.FilterIndex = 1;
			openDialog.CheckFileExists = true;
			openDialog.AddExtension = true;
			openDialog.DefaultExt = ".dcl";
			openDialog.Multiselect = false;
			openDialog.ShowHelp = false;
			openDialog.SupportMultiDottedExtensions = true;
			openDialog.Title = "Open Level";
			openDialog.ValidateNames = true;

			DialogResult result = openDialog.ShowDialog( this );

			if( DialogResult.OK == result )
			{
				this.listActors.ClearSelected();
				this.listLoadedModels.ClearSelected();

				string filename = openDialog.FileName;
				this.currentFile = filename;

				foreach( IBrowserListener listener in this.browserListeners )
				{
					listener.OnOpenLevel( filename );
				}
			}
		}

		private void saveLevelToolStripMenuItem_Click( object sender, EventArgs e )
		{
			this.SaveIfDirty( false );
		}

		private void saveAsToolStripMenuItem_Click( object sender, EventArgs e )
		{
			this.SaveLevel( true );
		}

		private void exitToolStripMenuItem_Click( object sender, EventArgs e )
		{
			this.Close();
		}

		private void deselectToolStripMenuItem_Click( object sender, EventArgs e )
		{
			this.DeselectAll();
		}

		private void btnLoadModel_Click( object sender, EventArgs e )
		{
			OpenFileDialog openDialog = new OpenFileDialog();
			openDialog.RestoreDirectory = true;
			openDialog.Filter = "Model Archives (*.spu)|*.spu|All files (*.*)|*.*";
			openDialog.FilterIndex = 1;
			openDialog.CheckFileExists = true;
			openDialog.AddExtension = true;
			openDialog.DefaultExt = ".spu";
			openDialog.Multiselect = false;
			openDialog.ShowHelp = false;
			openDialog.SupportMultiDottedExtensions = true;
			openDialog.Title = "Open Model File";
			openDialog.ValidateNames = true;

			DialogResult result = openDialog.ShowDialog( this );

			if( DialogResult.OK == openDialog.ShowDialog( this ) )
			{
				string modelFile = openDialog.FileName;

				foreach( IBrowserListener listener in this.browserListeners )
				{
					listener.OnLoadModel( modelFile );
				}
			}
		}

		private void btnCreateActor_Click( object sender, EventArgs e )
		{
			// Make sure a model is selected first
			if( this.listLoadedModels.SelectedIndex == -1 ) return;

			this.listActors.ClearSelected();

			Debug.Assert( this.listLoadedModels.SelectedItem is IContentObject );
			IContentObject model = (IContentObject)this.listLoadedModels.SelectedItem;
			foreach( IBrowserListener listener in this.browserListeners )
			{
				listener.OnActorCreated( model );
			}
		}

		private void btnDeleteActor_Click( object sender, EventArgs e )
		{
			// Make sure an actor is selected first
			if( this.listActors.SelectedIndex == -1 ) return;

			Debug.Assert( this.listActors.SelectedItem is IContentObject );
			IContentObject actor = (IContentObject)this.listActors.SelectedItem;
			this.listActors.ClearSelected();

			foreach( IBrowserListener listener in this.browserListeners )
			{
				listener.OnActorDeleted( actor );
			}
		}

		private void listActors_SelectedIndexChanged( object sender, EventArgs e )
		{
			if( this.listActors.SelectedIndex == -1 )
			{
				foreach( IBrowserListener listener in this.browserListeners )
				{
					listener.OnActorDeselected();
				}
			}
			else
			{
				Debug.Assert( this.listActors.SelectedItem is IContentObject );
				IContentObject actor = (IContentObject)this.listActors.SelectedItem;
				foreach( IBrowserListener listener in this.browserListeners )
				{
					listener.OnActorSelected( actor );
				}
			}
		}

		private void listLoadedModels_SelectedIndexChanged( object sender, EventArgs e )
		{
			if( this.listLoadedModels.SelectedIndex == -1 )
			{
				foreach( IBrowserListener listener in this.browserListeners )
				{
					listener.OnModelDeselected();
				}
			}
			else
			{
				Debug.Assert( this.listLoadedModels.SelectedItem is IContentObject );
				IContentObject model = (IContentObject)this.listLoadedModels.SelectedItem;
				foreach( IBrowserListener listener in this.browserListeners )
				{
					listener.OnModelSelected( model );
				}
			}
		}

		#endregion




		#region Internal functions

		internal void AddBrowserListener( IBrowserListener listener )
		{
			this.browserListeners.Add( listener );
		}

		internal void RemoveBrowserListener( IBrowserListener listener )
		{
			this.browserListeners.Remove( listener );
		}

		internal void AddModel( IContentObject model )
		{
			this.listLoadedModels.ClearSelected();
			this.listLoadedModels.Items.Add( model );
		}

		internal void RemoveModel( IContentObject model )
		{
			this.listLoadedModels.ClearSelected();
			this.listLoadedModels.Items.Remove( model );
		}

		internal void AddActor( IContentObject actor )
		{
			this.listActors.ClearSelected();
			this.listActors.Items.Add( actor );
		}

		internal void RemoveActor( IContentObject actor )
		{
			this.listActors.ClearSelected();
			this.listActors.Items.Remove( actor );
		}

		internal void SetDirtyFlag( bool isDirty )
		{
			this.isDirty = isDirty;

			this.Text = m_BaseTitle + ( isDirty ? " *" : "" );
		}

		internal void SetCurrentFile( string currentFile )
		{
			this.currentFile = currentFile;
		}

		#endregion
	}
}
