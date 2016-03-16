using System;
using System.Diagnostics;
using System.Threading;
using System.Windows.Forms;

namespace CS_Browser
{
	public class Browser
	{
		private BrowserForm browserWindow;
		private Thread formsThread;
		private bool running;

		public Browser()
		{
			Volatile.Write( ref this.running, false );

			this.formsThread = new Thread( () => this.Run() );
			this.formsThread.SetApartmentState( ApartmentState.STA );
			this.formsThread.Start();

			while( !Volatile.Read( ref running ) )
			{
				Thread.Yield();
			}
		}

		public void AddBrowserListener( IBrowserListener listener )
		{
			this.browserWindow.AddBrowserListener( listener );
		}

		public void RemoveBrowserListener( IBrowserListener listener )
		{
			this.browserWindow.RemoveBrowserListener( listener );
		}

		public void AddModel( IContentObject model )
		{
			this.browserWindow.Invoke( new Action( () => this.browserWindow.AddModel( model ) ) );
		}

		public void RemoveModel( IContentObject model )
		{
			this.browserWindow.Invoke( new Action( () => this.browserWindow.RemoveModel( model ) ) );
		}

		public void AddActor( IContentObject actor )
		{
			this.browserWindow.Invoke( new Action( () => this.browserWindow.AddActor( actor ) ) );
		}

		public void RemoveActor( IContentObject actor )
		{
			this.browserWindow.Invoke( new Action( () => this.browserWindow.RemoveActor( actor ) ) );
		}

		/// <summary>
		/// This function is only here to initiate an editor close event. The editor's main window should only close
		/// when the BrowserListener's OnExit callback is called. (This is to prevent the editor's main window from
		/// closing if the user cancels the save operation.)
		/// 
		/// This function may not be called from any of the browser listeners (unless they create a new thread to call it).
		/// </summary>
		public void Exit()
		{
			Debug.Assert( Thread.CurrentThread != this.formsThread );
			this.browserWindow.Invoke( new Action( () => this.browserWindow.Close() ) );
		}

		public void SetFileDirtyFlag( bool isDirty )
		{
			this.browserWindow.Invoke( new Action( () => this.browserWindow.SetDirtyFlag( isDirty ) ) );
		}

		public void SetCurrentFile( string currentFile )
		{
			this.browserWindow.SetCurrentFile( currentFile );
		}



		private void Run()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault( false );

			this.browserWindow = new BrowserForm();
			Volatile.Write( ref running, true );

			Application.Run( this.browserWindow );
		}
	}
}
