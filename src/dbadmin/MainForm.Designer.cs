namespace zuki.dbsfw
{
	partial class MainForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.m_menustrip = new System.Windows.Forms.MenuStrip();
			this.m_menufile = new System.Windows.Forms.ToolStripMenuItem();
			this.m_menufileexit = new System.Windows.Forms.ToolStripMenuItem();
			this.m_windowmenu = new System.Windows.Forms.ToolStripMenuItem();
			this.m_menuwindowcloseall = new System.Windows.Forms.ToolStripMenuItem();
			this.m_helpmenu = new System.Windows.Forms.ToolStripMenuItem();
			this.m_statusstrip = new System.Windows.Forms.StatusStrip();
			this.m_menustrip.SuspendLayout();
			this.SuspendLayout();
			// 
			// m_menustrip
			// 
			this.m_menustrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.m_menufile,
            this.m_windowmenu,
            this.m_helpmenu});
			this.m_menustrip.Location = new System.Drawing.Point(0, 0);
			this.m_menustrip.MdiWindowListItem = this.m_windowmenu;
			this.m_menustrip.Name = "m_menustrip";
			this.m_menustrip.Size = new System.Drawing.Size(1184, 24);
			this.m_menustrip.TabIndex = 0;
			this.m_menustrip.Text = "menuStrip1";
			// 
			// m_menufile
			// 
			this.m_menufile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.m_menufileexit});
			this.m_menufile.Name = "m_menufile";
			this.m_menufile.Size = new System.Drawing.Size(37, 20);
			this.m_menufile.Text = "&File";
			// 
			// m_menufileexit
			// 
			this.m_menufileexit.Name = "m_menufileexit";
			this.m_menufileexit.Size = new System.Drawing.Size(180, 22);
			this.m_menufileexit.Text = "E&xit";
			this.m_menufileexit.Click += new System.EventHandler(this.OnFileExit);
			// 
			// m_windowmenu
			// 
			this.m_windowmenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.m_menuwindowcloseall});
			this.m_windowmenu.Name = "m_windowmenu";
			this.m_windowmenu.Size = new System.Drawing.Size(63, 20);
			this.m_windowmenu.Text = "&Window";
			// 
			// m_menuwindowcloseall
			// 
			this.m_menuwindowcloseall.Name = "m_menuwindowcloseall";
			this.m_menuwindowcloseall.Size = new System.Drawing.Size(120, 22);
			this.m_menuwindowcloseall.Text = "&Close All";
			this.m_menuwindowcloseall.Click += new System.EventHandler(this.OnWindowCloseAll);
			// 
			// m_helpmenu
			// 
			this.m_helpmenu.Name = "m_helpmenu";
			this.m_helpmenu.Size = new System.Drawing.Size(44, 20);
			this.m_helpmenu.Text = "&Help";
			// 
			// m_statusstrip
			// 
			this.m_statusstrip.Location = new System.Drawing.Point(0, 739);
			this.m_statusstrip.Name = "m_statusstrip";
			this.m_statusstrip.Size = new System.Drawing.Size(1184, 22);
			this.m_statusstrip.TabIndex = 1;
			this.m_statusstrip.Text = "statusStrip1";
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
			this.ClientSize = new System.Drawing.Size(1184, 761);
			this.Controls.Add(this.m_statusstrip);
			this.Controls.Add(this.m_menustrip);
			this.IsMdiContainer = true;
			this.MainMenuStrip = this.m_menustrip;
			this.Name = "MainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Database Administration";
			this.Load += new System.EventHandler(this.OnLoad);
			this.m_menustrip.ResumeLayout(false);
			this.m_menustrip.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.MenuStrip m_menustrip;
		private System.Windows.Forms.ToolStripMenuItem m_menufile;
		private System.Windows.Forms.ToolStripMenuItem m_menufileexit;
		private System.Windows.Forms.StatusStrip m_statusstrip;
		private System.Windows.Forms.ToolStripMenuItem m_windowmenu;
		private System.Windows.Forms.ToolStripMenuItem m_helpmenu;
		private System.Windows.Forms.ToolStripMenuItem m_menuwindowcloseall;
	}
}

