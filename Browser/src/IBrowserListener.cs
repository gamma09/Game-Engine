using System;
namespace CS_Browser
{

	public interface IBrowserListener
	{
		void OnModelSelected( IContentObject model );
		void OnModelDeselected();

		void OnActorSelected( IContentObject actor );
		void OnActorDeselected();

		void OnActorCreated( IContentObject model );
		void OnActorDeleted( IContentObject actor );

		void OnExit();

		void OnNewLevel();
		void OnOpenLevel( string filepath );
		void OnSaveLevel( string filepath );

		void OnLoadModel( string filepath );
	}
}
