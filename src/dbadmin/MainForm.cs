//---------------------------------------------------------------------------
// Copyright (c) 2025 Michael G. Brehm
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------

using System;
using System.Windows.Forms;

using zuki.dbsfw.data;

namespace zuki.dbsfw
{
	/// <summary>
	/// Implements the main application form
	/// </summary>
	public partial class MainForm : Form
	{
		/// <summary>
		/// Instance Constructor
		/// </summary>
		public MainForm()
		{
			InitializeComponent();
		}

		/// <summary>
		/// Clean up any resources being used
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed</param>
		protected override void Dispose(bool disposing)
		{
			if(disposing && (components != null))
			{
				m_database?.Dispose();
				components.Dispose();
			}

			base.Dispose(disposing);
		}

		//-------------------------------------------------------------------
		// Event Handlers
		//-------------------------------------------------------------------

		/// <summary>
		/// Invoked when the File/Exit menu option has been selected
		/// </summary>
		/// <param name="sender">Object raising this event</param>
		/// <param name="args">Standard event arguments</param>
		private void OnFileExit(object sender, EventArgs args)
		{
			OnWindowCloseAll(this, EventArgs.Empty);
			CloseDatabase();
			Close();
		}

		/// <summary>
		/// Invoked when the form is being loaded
		/// </summary>
		/// <param name="sender">Object raising this event</param>
		/// <param name="args">Standard event arguments</param>
		private void OnLoad(object sender, EventArgs args)
		{
		}

		/// <summary>
		/// Invoked when the Window/Close All menu option has been selected
		/// </summary>
		/// <param name="sender">Object raising this event</param>
		/// <param name="args">Standard event arguments</param>
		private void OnWindowCloseAll(object sender, EventArgs args)
		{
			foreach(var child in MdiChildren)
			{
				child.Close();
				child.Dispose();
			}
		}

		//---------------------------------------------------------------------------
		// Private Member Functions
		//---------------------------------------------------------------------------

		/// <summary>
		/// Checks if an MDI child of the specified type already exists and activates it
		/// </summary>
		/// <param name="type">Type of the MDI child to check for</param>
		/// <returns>Flag indicating if an MDI child of the specified type existed</returns>
		private bool ActivateExistingMDIChild(Type type)
		{
			foreach(var child in MdiChildren)
			{
				if(child.GetType() == type)
				{
					child.Show();
					child.Activate();
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// Closes the database
		/// </summary>
		private void CloseDatabase()
		{
			m_database?.Dispose();
			m_database = null;
		}

		//---------------------------------------------------------------------------
		// Member Variables
		//---------------------------------------------------------------------------

		/// <summary>
		/// The database instance
		/// </summary>
		private Database m_database = null;
	}
}
