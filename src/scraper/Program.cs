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
using System.IO;
using System.Text;

using Newtonsoft.Json;

namespace zuki.dbsfw
{
	internal class Program
	{
		/// <summary>
		/// Application entry point
		/// </summary>
		/// <param name="arguments">Array of command line arguments</param>
		static int Main(string[] arguments)
		{
			try
			{
				// Parse the command line arguments and switches
				CommandLine commandline = new CommandLine(arguments);

				// If no arguments were specified or help was requested, display the usage and exit
				if((commandline.Arguments.Count == 0) || commandline.Switches.ContainsKey("?"))
				{
					ShowUsage();
					return 0;
				}

				// There has to be exactly two command line arguments
				if(commandline.Arguments.Count != 2) throw new ArgumentException("Invalid command line arguments");
				string command = commandline.Arguments[0].ToUpper();
				string target = commandline.Arguments[1].ToUpper();

				// The output directory defaults to the current directory
				string outdir = Path.GetFullPath(Environment.CurrentDirectory);
				if(commandline.Switches.ContainsKey("outdir"))
					outdir = Path.GetFullPath(commandline.Switches["outdir"]);

				// Attempt to create the output directory if it does not exist
				if(!Directory.Exists(outdir)) Directory.CreateDirectory(outdir);

				// CARD
				if(command == "CARD") ScrapeCard(target, Path.Combine(outdir, "card"));

				else throw new ArgumentException("Command " + command + " is not recognized");
			}

			catch(Exception ex)
			{
				Console.WriteLine();
				Console.WriteLine("ERROR: " + ex.Message);
				Console.WriteLine();

				return unchecked((int)0x80004005);      // <-- E_FAIL
			}

			return 0;
		}

		//-------------------------------------------------------------------
		// Private Member Functions
		//-------------------------------------------------------------------

		/// <summary>
		/// Writes a ScrapedCardDetail object into the output stream
		/// </summary>
		/// <param name="side">Side for leader cards</param>
		/// <param name="writer">JsonWriter instance</param>
		/// <param name="language">Detail language</param>
		/// <param name="detail">Detail object</param>
		private static void WriteCardDetailObject(string side, string language, JsonWriter writer, 
			Scraper.ScrapedCardDetail detail)
		{
			if(string.IsNullOrEmpty(language)) throw new ArgumentNullException(nameof(language));
			if(writer == null) throw new ArgumentNullException(nameof(writer));
			
			// {
			writer.WriteStartObject();

			// side: [null]
			writer.WritePropertyName("side");
			if(side != null) writer.WriteValue(side);
			else writer.WriteNull();

			// language:
			writer.WritePropertyName("language");
			writer.WriteValue(language);

			// name: [null]
			writer.WritePropertyName("name");
			if(detail.Name != null) writer.WriteValue(detail.Name);
			else writer.WriteNull();

			// cost: [null]
			writer.WritePropertyName("cost");
			if(detail.Cost.HasValue) writer.WriteValue(detail.Cost);
			else writer.WriteNull();

			// specifiedcost: [null]
			writer.WritePropertyName("specifiedcost");
			if(detail.SpecifiedCost != null) writer.WriteValue(detail.SpecifiedCost);
			else writer.WriteNull();

			// power: [null]
			writer.WritePropertyName("power");
			if(detail.Power.HasValue) writer.WriteValue(detail.Power);
			else writer.WriteNull();

			// combopower: [null]
			writer.WritePropertyName("combopower");
			if(detail.ComboPower.HasValue) writer.WriteValue(detail.ComboPower);
			else writer.WriteNull();

			// traits: [null]
			writer.WritePropertyName("traits");
			if(detail.Traits != null) writer.WriteValue(detail.Traits);
			else writer.WriteNull();

			// effect: [null]
			writer.WritePropertyName("effect");
			if(detail.Effect != null) writer.WriteValue(detail.Effect);
			else writer.WriteNull();

			// }
			writer.WriteEndObject();
		}

		/// <summary>
		/// Writes a ScrapedCardFAQ object into the output stream
		/// </summary>
		/// <param name="language">FAQ language</param>
		/// <param name="writer">JsonWriter instance</param>
		/// <param name="faq">FAQ object</param>
		private static void WriteCardFaqObject(string language, JsonWriter writer, Scraper.ScrapedCardFAQ faq)
		{
			if(string.IsNullOrEmpty(language)) throw new ArgumentNullException(nameof(language));
			if(writer == null) throw new ArgumentNullException(nameof(writer));

			// {
			writer.WriteStartObject();

			// faqid: [null]
			writer.WritePropertyName("faqid");
			if(faq.ID != null) writer.WriteValue(faq.ID);
			else writer.WriteNull();

			// language:
			writer.WritePropertyName("language");
			writer.WriteValue(language);

			// question: [null]
			writer.WritePropertyName("question");
			if(faq.Question != null) writer.WriteValue(faq.Question);
			else writer.WriteNull();

			// answer: [null]
			writer.WritePropertyName("answer");
			if(faq.Answer != null) writer.WriteValue(faq.Answer);
			else writer.WriteNull();

			// related: [null]
			writer.WritePropertyName("related");
			if((faq.Related != null) && (faq.Related.Count > 0))
			{
				writer.WriteStartArray();

				foreach(string related in faq.Related)
					writer.WriteValue(related);

				writer.WriteEndArray();
			}
			else writer.WriteNull();

			// }
			writer.WriteEndObject();
		}

		/// <summary>
		/// Writes a ScrapedCardFAQ object into the output stream
		/// </summary>
		/// <param name="side">Side for leader cards</param>
		/// <param name="language">Image language</param>
		/// <param name="writer">JsonWriter instance</param>
		/// <param name="image">Image object</param>
		private static void WriteCardImageObject(string side, string language, JsonWriter writer,
			Scraper.ScrapedCardImage image)
		{
			if(string.IsNullOrEmpty(language)) throw new ArgumentNullException(nameof(language));
			if(writer == null) throw new ArgumentNullException(nameof(writer));

			// {
			writer.WriteStartObject();

			// side: [null]
			writer.WritePropertyName("side");
			if(side != null) writer.WriteValue(side);
			else writer.WriteNull();

			// language:
			writer.WritePropertyName("language");
			writer.WriteValue(language);

			// format:
			writer.WritePropertyName("format");
			writer.WriteValue(image.Format);

			// image: [null]
			writer.WritePropertyName("image");
			if(image.Image.Length > 0) writer.WriteValue(Convert.ToBase64String(image.Image));
			else writer.WriteNull();

			// }
			writer.WriteEndObject();
		}

		/// <summary>
		/// Scrapes a card into the specified output directory
		/// </summary>
		/// <param name="target">Target card code</param>
		/// <param name="outdir">Output directory</param>
		private static void ScrapeCard(string target, string outdir)
		{
			// TODO: not all cards have EN and JP, like FP-030; let it fail for now
			// I'm not going to scrape all the cards at first, just a handful

			var en = Scraper.ScrapeCard(target, "EN");
			var jp = Scraper.ScrapeCard(target, "JP");

			StringBuilder sb = new StringBuilder();
			StringWriter sw = new StringWriter(sb);
			sw.NewLine = "\n";

			using(JsonWriter writer = new JsonTextWriter(sw))
			{
				writer.Formatting = Formatting.Indented;

				writer.WriteStartObject();

				writer.WritePropertyName("cardid");
				writer.WriteValue(target.ToUpper());
				writer.WritePropertyName("type");
				writer.WriteValue(en.Type);
				writer.WritePropertyName("color");
				writer.WriteValue(en.FrontDetail.Color);
				writer.WritePropertyName("rarity");
				writer.WriteValue(en.Rarity);

				// detail: []
				writer.WritePropertyName("detail");
				writer.WriteStartArray();

				if(en.Type == "LEADER")
				{
					if(!en.BackDetail.HasValue) throw new Exception("Leader card missing EN back detail");
					WriteCardDetailObject("FRONT", "EN", writer, en.FrontDetail);
					WriteCardDetailObject("BACK", "EN", writer, en.BackDetail.Value);

					if(!jp.BackDetail.HasValue) throw new Exception("Leader card missing JP back detail");
					WriteCardDetailObject("FRONT", "JP", writer, jp.FrontDetail);
					WriteCardDetailObject("BACK", "JP", writer, jp.BackDetail.Value);
				}
				else
				{
					WriteCardDetailObject(null, "EN", writer, en.FrontDetail);
					WriteCardDetailObject(null, "JP", writer, jp.FrontDetail);
				}

				writer.WriteEndArray();

				// faq: [null]
				writer.WritePropertyName("faq");
				bool hasenfaq = (en.FAQ != null) && (en.FAQ.Count > 0);
				bool hasjpfaq = (jp.FAQ != null) && (jp.FAQ.Count > 0);
				if(hasenfaq || hasjpfaq)
				{

					writer.WriteStartArray();

					if(hasenfaq)
					{
						foreach(Scraper.ScrapedCardFAQ faq in en.FAQ)
							WriteCardFaqObject("EN", writer, faq);
					}

					if(hasjpfaq)
					{
						foreach(Scraper.ScrapedCardFAQ faq in jp.FAQ)
							WriteCardFaqObject("JP", writer, faq);
					}

					writer.WriteEndArray();
				}
				else writer.WriteNull();

				// image: []
				writer.WritePropertyName("image");
				writer.WriteStartArray();

				if(en.Type == "LEADER")
				{
					if(!en.BackImage.HasValue) throw new Exception("Leader card missing EN back image"); 
					WriteCardImageObject("FRONT", "EN", writer, en.FrontImage);
					WriteCardImageObject("BACK", "EN", writer, en.BackImage.Value);

					if(!jp.BackImage.HasValue) throw new Exception("Leader card missing JP back image");
					WriteCardImageObject("FRONT", "JP", writer, jp.FrontImage);
					WriteCardImageObject("BACK", "JP", writer, jp.BackImage.Value);
				}
				else
				{
					WriteCardImageObject(null, "EN", writer, en.FrontImage);
					WriteCardImageObject(null, "JP", writer, jp.FrontImage);
				}

				writer.WriteEndArray();

				writer.WriteEndObject();
			}

			using(var output = File.CreateText(Path.Combine(outdir, target.ToUpper() + ".json")))
			{
				output.Write(sb.ToString());
				output.Flush();
			}

		}

		/// <summary>
		/// Shows application usage information
		/// </summary>
		private static void ShowUsage()
		{
			Console.WriteLine(AppDomain.CurrentDomain.FriendlyName.ToUpper() + " todo");
			Console.WriteLine();
			Console.WriteLine("scraper - todo options here when done");
		}

	}
}
