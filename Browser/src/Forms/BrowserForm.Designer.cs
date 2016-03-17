namespace CS_Browser
{
	partial class BrowserForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Windows.Forms.MenuStrip menuStrip1;
			System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
			System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
			System.Windows.Forms.ToolStripMenuItem editToolStripMenu;
			System.Windows.Forms.TabControl tabControl1;
			System.Windows.Forms.TabPage tabPage1;
			System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
			System.Windows.Forms.TabPage tabPage2;
			System.Windows.Forms.FlowLayoutPanel flowLayoutPanel2;
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BrowserForm));
			this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.openLevelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.saveLevelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.deselectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.listLoadedModels = new System.Windows.Forms.ListBox();
			this.btnLoadModel = new System.Windows.Forms.Button();
			this.btnCreateActor = new System.Windows.Forms.Button();
			this.listActors = new System.Windows.Forms.ListBox();
			this.btnDeleteActor = new System.Windows.Forms.Button();
			menuStrip1 = new System.Windows.Forms.MenuStrip();
			fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			editToolStripMenu = new System.Windows.Forms.ToolStripMenuItem();
			tabControl1 = new System.Windows.Forms.TabControl();
			tabPage1 = new System.Windows.Forms.TabPage();
			flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
			tabPage2 = new System.Windows.Forms.TabPage();
			flowLayoutPanel2 = new System.Windows.Forms.FlowLayoutPanel();
			menuStrip1.SuspendLayout();
			tabControl1.SuspendLayout();
			tabPage1.SuspendLayout();
			flowLayoutPanel1.SuspendLayout();
			tabPage2.SuspendLayout();
			flowLayoutPanel2.SuspendLayout();
			this.SuspendLayout();
			// 
			// menuStrip1
			// 
			menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            fileToolStripMenuItem,
            editToolStripMenu});
			menuStrip1.Location = new System.Drawing.Point(0, 0);
			menuStrip1.Name = "menuStrip1";
			menuStrip1.Size = new System.Drawing.Size(533, 24);
			menuStrip1.TabIndex = 1;
			menuStrip1.Text = "menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openLevelToolStripMenuItem,
            this.saveLevelToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            toolStripSeparator1,
            this.exitToolStripMenuItem});
			fileToolStripMenuItem.Name = "fileToolStripMenuItem";
			fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
			fileToolStripMenuItem.Text = "&File";
			// 
			// newToolStripMenuItem
			// 
			this.newToolStripMenuItem.Name = "newToolStripMenuItem";
			this.newToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
			this.newToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.newToolStripMenuItem.Text = "&New";
			this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
			// 
			// openLevelToolStripMenuItem
			// 
			this.openLevelToolStripMenuItem.Name = "openLevelToolStripMenuItem";
			this.openLevelToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
			this.openLevelToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.openLevelToolStripMenuItem.Text = "&Open";
			this.openLevelToolStripMenuItem.Click += new System.EventHandler(this.openLevelToolStripMenuItem_Click);
			// 
			// saveLevelToolStripMenuItem
			// 
			this.saveLevelToolStripMenuItem.Name = "saveLevelToolStripMenuItem";
			this.saveLevelToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
			this.saveLevelToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.saveLevelToolStripMenuItem.Text = "&Save";
			this.saveLevelToolStripMenuItem.Click += new System.EventHandler(this.saveLevelToolStripMenuItem_Click);
			// 
			// saveAsToolStripMenuItem
			// 
			this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
			this.saveAsToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Alt) 
            | System.Windows.Forms.Keys.S)));
			this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.saveAsToolStripMenuItem.Text = "Save &As";
			this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
			// 
			// toolStripSeparator1
			// 
			toolStripSeparator1.Name = "toolStripSeparator1";
			toolStripSeparator1.Size = new System.Drawing.Size(174, 6);
			// 
			// exitToolStripMenuItem
			// 
			this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
			this.exitToolStripMenuItem.ShortcutKeyDisplayString = "Alt+F4";
			this.exitToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.exitToolStripMenuItem.Text = "E&xit";
			this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
			// 
			// editToolStripMenu
			// 
			editToolStripMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deselectToolStripMenuItem});
			editToolStripMenu.Name = "editToolStripMenu";
			editToolStripMenu.Size = new System.Drawing.Size(39, 20);
			editToolStripMenu.Text = "&Edit";
			// 
			// deselectToolStripMenuItem
			// 
			this.deselectToolStripMenuItem.Name = "deselectToolStripMenuItem";
			this.deselectToolStripMenuItem.ShortcutKeyDisplayString = "Esc";
			this.deselectToolStripMenuItem.Size = new System.Drawing.Size(142, 22);
			this.deselectToolStripMenuItem.Text = "&Deselect";
			this.deselectToolStripMenuItem.Click += new System.EventHandler(this.deselectToolStripMenuItem_Click);
			// 
			// tabControl1
			// 
			tabControl1.Controls.Add(tabPage1);
			tabControl1.Controls.Add(tabPage2);
			tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
			tabControl1.Location = new System.Drawing.Point(0, 24);
			tabControl1.Name = "tabControl1";
			tabControl1.SelectedIndex = 0;
			tabControl1.Size = new System.Drawing.Size(533, 491);
			tabControl1.TabIndex = 2;
			// 
			// tabPage1
			// 
			tabPage1.Controls.Add(this.listLoadedModels);
			tabPage1.Controls.Add(flowLayoutPanel1);
			tabPage1.Location = new System.Drawing.Point(4, 22);
			tabPage1.Name = "tabPage1";
			tabPage1.Padding = new System.Windows.Forms.Padding(3);
			tabPage1.Size = new System.Drawing.Size(525, 465);
			tabPage1.TabIndex = 0;
			tabPage1.Text = "Models";
			tabPage1.UseVisualStyleBackColor = true;
			// 
			// listLoadedModels
			// 
			this.listLoadedModels.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listLoadedModels.FormattingEnabled = true;
			this.listLoadedModels.Location = new System.Drawing.Point(3, 3);
			this.listLoadedModels.Name = "listLoadedModels";
			this.listLoadedModels.Size = new System.Drawing.Size(519, 430);
			this.listLoadedModels.Sorted = true;
			this.listLoadedModels.TabIndex = 1;
			this.listLoadedModels.SelectedIndexChanged += new System.EventHandler(this.listLoadedModels_SelectedIndexChanged);
			// 
			// flowLayoutPanel1
			// 
			flowLayoutPanel1.AutoSize = true;
			flowLayoutPanel1.Controls.Add(this.btnLoadModel);
			flowLayoutPanel1.Controls.Add(this.btnCreateActor);
			flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Bottom;
			flowLayoutPanel1.Location = new System.Drawing.Point(3, 433);
			flowLayoutPanel1.Name = "flowLayoutPanel1";
			flowLayoutPanel1.Size = new System.Drawing.Size(519, 29);
			flowLayoutPanel1.TabIndex = 0;
			// 
			// btnLoadModel
			// 
			this.btnLoadModel.AutoSize = true;
			this.btnLoadModel.Location = new System.Drawing.Point(3, 3);
			this.btnLoadModel.Name = "btnLoadModel";
			this.btnLoadModel.Size = new System.Drawing.Size(75, 23);
			this.btnLoadModel.TabIndex = 0;
			this.btnLoadModel.Text = "Load Model";
			this.btnLoadModel.UseVisualStyleBackColor = true;
			this.btnLoadModel.Click += new System.EventHandler(this.btnLoadModel_Click);
			// 
			// btnCreateActor
			// 
			this.btnCreateActor.AutoSize = true;
			this.btnCreateActor.Location = new System.Drawing.Point(84, 3);
			this.btnCreateActor.Name = "btnCreateActor";
			this.btnCreateActor.Size = new System.Drawing.Size(76, 23);
			this.btnCreateActor.TabIndex = 1;
			this.btnCreateActor.Text = "Create Actor";
			this.btnCreateActor.UseVisualStyleBackColor = true;
			this.btnCreateActor.Click += new System.EventHandler(this.btnCreateActor_Click);
			// 
			// tabPage2
			// 
			tabPage2.Controls.Add(this.listActors);
			tabPage2.Controls.Add(flowLayoutPanel2);
			tabPage2.Location = new System.Drawing.Point(4, 22);
			tabPage2.Name = "tabPage2";
			tabPage2.Padding = new System.Windows.Forms.Padding(3);
			tabPage2.Size = new System.Drawing.Size(525, 465);
			tabPage2.TabIndex = 1;
			tabPage2.Text = "Actors";
			tabPage2.UseVisualStyleBackColor = true;
			// 
			// listActors
			// 
			this.listActors.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listActors.FormattingEnabled = true;
			this.listActors.Location = new System.Drawing.Point(3, 3);
			this.listActors.Name = "listActors";
			this.listActors.Size = new System.Drawing.Size(519, 430);
			this.listActors.Sorted = true;
			this.listActors.TabIndex = 1;
			this.listActors.SelectedIndexChanged += new System.EventHandler(this.listActors_SelectedIndexChanged);
			// 
			// flowLayoutPanel2
			// 
			flowLayoutPanel2.AutoSize = true;
			flowLayoutPanel2.Controls.Add(this.btnDeleteActor);
			flowLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Bottom;
			flowLayoutPanel2.Location = new System.Drawing.Point(3, 433);
			flowLayoutPanel2.Name = "flowLayoutPanel2";
			flowLayoutPanel2.Size = new System.Drawing.Size(519, 29);
			flowLayoutPanel2.TabIndex = 0;
			// 
			// btnDeleteActor
			// 
			this.btnDeleteActor.AutoSize = true;
			this.btnDeleteActor.Location = new System.Drawing.Point(3, 3);
			this.btnDeleteActor.Name = "btnDeleteActor";
			this.btnDeleteActor.Size = new System.Drawing.Size(76, 23);
			this.btnDeleteActor.TabIndex = 0;
			this.btnDeleteActor.Text = "Delete Actor";
			this.btnDeleteActor.UseVisualStyleBackColor = true;
			this.btnDeleteActor.Click += new System.EventHandler(this.btnDeleteActor_Click);
			// 
			// BrowserForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(533, 515);
			this.Controls.Add(tabControl1);
			this.Controls.Add(menuStrip1);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MainMenuStrip = menuStrip1;
			this.Name = "BrowserForm";
			this.ShowInTaskbar = false;
			this.Text = "Browser";
			this.Load += new System.EventHandler(this.BrowserForm_Load);
			menuStrip1.ResumeLayout(false);
			menuStrip1.PerformLayout();
			tabControl1.ResumeLayout(false);
			tabPage1.ResumeLayout(false);
			tabPage1.PerformLayout();
			flowLayoutPanel1.ResumeLayout(false);
			flowLayoutPanel1.PerformLayout();
			tabPage2.ResumeLayout(false);
			tabPage2.PerformLayout();
			flowLayoutPanel2.ResumeLayout(false);
			flowLayoutPanel2.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListBox listLoadedModels;
		private System.Windows.Forms.ListBox listActors;
		private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem openLevelToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem saveLevelToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem deselectToolStripMenuItem;
		private System.Windows.Forms.Button btnLoadModel;
		private System.Windows.Forms.Button btnCreateActor;
		private System.Windows.Forms.Button btnDeleteActor;
	}
}

