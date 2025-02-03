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
using System.Collections.Generic;
using System.Net;
using System.Text;
using System.Text.RegularExpressions;

namespace zuki.dbsfw
{
	/// <summary>
	/// Implements scraper functions
	/// </summary>
	internal static class Scraper
	{
		/// <summary>
		/// Base URL for retrieving card information from the Bandai web site
		/// </summary>
		static private readonly string CARD_DETAIL_URL_FORMAT = "https://www.dbs-cardgame.com/fw/{0}/cardlist/detail.php?card_no={1}";

		/// <summary>
		/// Base URL for retrieving a card image from the Bandai web site
		/// </summary>
		static private readonly string CARD_IMAGE_URL_FORMAT = "https://www.dbs-cardgame.com/fw/images/cards/card/{0}/{1}";

		//-------------------------------------------------------------------------
		// Public Data Types
		//-------------------------------------------------------------------------

		/// <summary>
		/// Scraped card information
		/// </summary>
		public struct ScrapedCard
		{
			public string Type;
			public string Rarity;
			public ScrapedCardDetail FrontDetail;
			public ScrapedCardImage FrontImage;
			public ScrapedCardDetail? BackDetail;
			public ScrapedCardImage? BackImage;
			public List<ScrapedCardFAQ> FAQ;
		}

		/// <summary>
		/// Scraped card detail information
		/// </summary>
		public struct ScrapedCardDetail
		{
			public string Name;
			public string Type;
			public string Color;
			public int? Cost;
			public string SpecifiedCost;
			public int? Power;
			public int? ComboPower;
			public string Traits;
			public string Effect;
		}

		/// <summary>
		/// Scraped card FAQ information
		/// </summary>
		public struct ScrapedCardFAQ
		{
			public string ID;
			public string Question;
			public string Answer;
			public List<string> Related;
		}

		/// <summary>
		/// Scraped card image information
		/// </summary>
		public struct ScrapedCardImage
		{
			public string Format;
			public byte[] Image;
		}

		//-------------------------------------------------------------------------
		// Member Functions
		//-------------------------------------------------------------------------

		/// <summary>
		/// Scrapes information about a specific card
		/// </summary>
		/// <param name="code">Card code</param>
		/// <param name="language">Language code</param>
		static public ScrapedCard ScrapeCard(string code, string language)
		{
			if(string.IsNullOrEmpty(code)) throw new ArgumentNullException(nameof(code));
			if(string.IsNullOrEmpty(language)) throw new ArgumentNullException(nameof(language));
			code = code.ToUpper();
			language = language.ToLower();

			string html = ScrapeHTML(string.Format(CARD_DETAIL_URL_FORMAT, language, code));

			const string CARDNOCOL_REGEX = "<div class=\"cardNoCol\">.*?<div class=\"cardNo\">(?<cardno>.*?)</div>.*?<div class=\"rarity\">(?<rarity>.*?)</div>";
			Match cardnocol = Regex.Match(html, CARDNOCOL_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!cardnocol.Groups["cardno"].Success || !cardnocol.Groups["rarity"].Success)
				throw new Exception("Unable to parse cardNoCol <div> in source HTML");

			// Make sure the code matches the one specified for sanity purposes
			string cardno = cardnocol.Groups["cardno"].Value.ToUpper();
			if(cardno != code) throw new Exception("cardNoCol <div> contains unexpected code " + cardno);

			ScrapedCard card = new ScrapedCard();
			
			if(cardnocol.Groups["rarity"].Value.Trim().ToUpper() == "L")
			{
				// Leader
				var details = ScrapeLeaderCardDetails(html);

				card.Type = details.Item1.Type;
				card.Rarity = cardnocol.Groups["rarity"].Value.Trim().ToUpper();
				card.FrontDetail = details.Item1;
				card.BackDetail = details.Item2;

				card.FrontImage = new ScrapedCardImage()
				{
					Format = "image/webp",
					Image = ScrapeCardImage(language, cardno + "_f.webp")
				};

				card.BackImage = new ScrapedCardImage()
				{
					Format = "image/webp",
					Image = ScrapeCardImage(language, cardno + "_b.webp")
				};
			}
			else
			{
				// Battle/Extra
				var detail = ScrapeCardDetail(html);

				card.Type = detail.Type;
				card.Rarity = cardnocol.Groups["rarity"].Value.Trim().ToUpper();
				card.FrontDetail = detail;

				card.FrontImage = new ScrapedCardImage()
				{
					Format = "image/webp",
					Image = ScrapeCardImage(language, cardno + ".webp")
				};
			}

			card.FAQ = ScrapeCardFAQs(html);

			return card;
		}

		//-------------------------------------------------------------------
		// Private Member Functions
		//-------------------------------------------------------------------

		/// <summary>
		/// Helper to deal with string inputs of "-"
		/// </summary>
		/// <param name="input">Input string</param>
		private static string ParseNullableString(string input)
		{
			if(string.IsNullOrEmpty(input)) return null;
			if(input == "-") return null;
			return input;
		}

		/// <summary>
		/// Helper to deal with numeric inputs of "-"
		/// </summary>
		/// <param name="input">Input string</param>
		private static int? ParseNullableInteger(string input)
		{
			if(string.IsNullOrEmpty(input)) return null;
			if(input == "-") return null;
			if(!int.TryParse(input, out int val)) return null;
			return val;
		}

		/// <summary>
		/// Scrapes the detail information for a non-leader card
		/// </summary>
		/// <param name="html">Input HTML</param>
		static private ScrapedCardDetail ScrapeCardDetail(string html)
		{
			if(string.IsNullOrEmpty(html)) throw new ArgumentNullException(nameof(html));

			// >> Name
			const string CARDNAME_REGEX = "<div class=\"nameCol\">.*?<.*?class=\"cardName\">(?<name>.*?)</";
			Match name = Regex.Match(html, CARDNAME_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!name.Groups["name"].Success) throw new Exception("Unable to capture name; check regex");

			// >> Type
			const string TYPE_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Card Type|カードタイプ)</.*?>.*?<div class=\"data\">(?<type>.*?)</div>";
			Match type = Regex.Match(html, TYPE_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!type.Groups["type"].Success) throw new Exception("Unable to capture type; check regex");

			// >> Color
			const string COLOR_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Color|色)</.*?>.*?<div class=\"data color.*?\">.*?<div class=\"colValue\" data-color=\"(?<color>.*?)\">";
			Match color = Regex.Match(html, COLOR_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!color.Groups["color"].Success) throw new Exception("Unable to capture color; check regex");

			// >> Cost
			const string COST_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Cost|コスト)</.*?>.*?<div class=\"data\">(?<cost>.*?)</div>";
			Match cost = Regex.Match(html, COST_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!cost.Groups["cost"].Success) throw new Exception("Unable to capture cost; check regex");

			// >> Specified Cost
			const string SPECIFIEDCOST_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Specified cost|指定コスト).*?<span class=\".*?\">(?<specifiedcost>.*?)</span>";
			Match specifiedcost = Regex.Match(html, SPECIFIEDCOST_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!specifiedcost.Groups["specifiedcost"].Success) specifiedcost = null; // <-- no match if specified cost is "-"

			// >> Power
			const string POWER_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Power|パワー)</.*?>.*?<div class=\"data\">(?<power>.*?)</div>";
			Match power = Regex.Match(html, POWER_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!power.Groups["power"].Success) throw new Exception("Unable to capture power; check regex");

			// >> Combo Power
			const string COMBOPOWER_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Combo power|コンボパワー)</.*?>.*?<div class=\"data\">(?<combopower>.*?)</div>";
			Match combopower = Regex.Match(html, COMBOPOWER_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!combopower.Groups["combopower"].Success) throw new Exception("Unable to capture combo power; check regex");

			// >> Features
			const string FEATURES_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Features|特徴)</.*?>.*?<div class=\"data\">(?<features>.*?)</div>";
			Match features = Regex.Match(html, FEATURES_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!features.Groups["features"].Success) throw new Exception("Unable to capture features; check regex");

			// >> Effect
			const string EFFECT_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Effect|カード効果).*?<div class=\"data.*?\">(?<effect>.*?)</div";
			Match effect = Regex.Match(html, EFFECT_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!effect.Groups["effect"].Success) throw new Exception("Unable to capture effect; check regex");

			// All non-leader cards have a cost, when scraping "-" means zero
			string coststr = cost.Groups["cost"].Value.Trim();
			int costval = 0;
			if((coststr != "-") && !int.TryParse(coststr, out costval)) throw new Exception("Unable to convert cost into an integer");

			return new ScrapedCardDetail
			{
				Type = type.Groups["type"].Value.Trim(),
				Color = color.Groups["color"].Value.Trim(),
				Cost = costval,
				SpecifiedCost = specifiedcost?.Groups["specifiedcost"].Value.Trim(),
				Name = name.Groups["name"].Value.Trim(),
				Power = ParseNullableInteger(power.Groups["power"].Value.Trim()),
				ComboPower = ParseNullableInteger(combopower.Groups["combopower"].Value.Trim()),
				Traits = ParseNullableString(features.Groups["features"].Value.Trim()),
				Effect = ParseNullableString(WebUtility.HtmlDecode(effect.Groups["effect"].Value.Trim()).Replace("<br>", "\n"))
			};
		}

		/// <summary>
		/// Scrapes Q&A items for a card
		/// </summary>
		/// <param name="html">Input HTML</param>
		static private List<ScrapedCardFAQ> ScrapeCardFAQs(string html)
		{
			if(string.IsNullOrEmpty(html)) throw new ArgumentNullException(nameof(html));

			const string FAQ_REGEX = "<li class=\"cardQA_listItem\">.*?cardQA_question\">.*?cardQA_number\">(?<number>.*?)</p>" +
				".*?cardQA_text\">(?<question>.*?)</p>.*?(cardQA_answer\">.*?cardQA_text\">(?<answer>.*?)</p>.*?)?</li>";
			MatchCollection faqs = Regex.Matches(html, FAQ_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(faqs.Count == 0) return null;

			List<ScrapedCardFAQ> faqlist = new List<ScrapedCardFAQ>();
			foreach(Match faq in faqs)
			{
				if(!faq.Groups["number"].Success || !faq.Groups["question"].Success) throw new Exception("Unable to capture FAQ; check regex");

				var scrapedfaq = new ScrapedCardFAQ()
				{
					ID = faq.Groups["number"].Value.Trim(),
					Question = WebUtility.HtmlDecode(faq.Groups["question"].Value.Trim()),
					Answer = (faq.Groups["answer"].Success) ? WebUtility.HtmlDecode(faq.Groups["answer"].Value.Trim()) : null
				};

				const string RELATEDITEM_REGEX = "<a class=\"cardQA_relatedItem\".*?href=.*?q=(?<cardid>.*?)#cardResult\".*?</a>";
				MatchCollection relateditems = Regex.Matches(faq.Value, RELATEDITEM_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
				if(relateditems.Count > 0)
				{
					scrapedfaq.Related = new List<string>();
					foreach(Match relateditem in relateditems)
					{
						if(!relateditem.Groups["cardid"].Success) throw new Exception("Unable to capture related card; check regex");
						scrapedfaq.Related.Add(relateditem.Groups["cardid"].Value.Trim());
					}
				}

				faqlist.Add(scrapedfaq);
			}

			return faqlist;
		}

		/// <summary>
		/// Scrapes the image for a card
		/// </summary>
		/// <param name="language">Language code</param>
		/// <param name="filename">Card image file name</param>
		static private byte[] ScrapeCardImage(string language, string filename)
		{
			using(WebClient client = new WebClient())
			{
				return client.DownloadData(string.Format(CARD_IMAGE_URL_FORMAT, language, filename));
			}
		}

		/// <summary>
		/// Scrapes an HTML page from a URL
		/// </summary>
		/// <param name="url">URL to be scraped</param>
		static private string ScrapeHTML(string url)
		{
			// Return the URL data as a UTF-8 encoded string
			using(WebClient client = new WebClient())
			{
				return Encoding.UTF8.GetString(client.DownloadData(url));
			}
		}

		/// <summary>
		/// Scrapes the detail information for a leader card
		/// </summary>
		/// <param name="html">Input HTML</param>
		static private Tuple<ScrapedCardDetail, ScrapedCardDetail> ScrapeLeaderCardDetails(string html)
		{
			if(string.IsNullOrEmpty(html)) throw new ArgumentNullException(nameof(html));

			// >> Front Name
			const string CARDNAME_FRONT_REGEX = "<div class=\"nameCol\">.*?<.*?class=\"cardName is-front\">(?<name>.*?)</";
			Match namefront = Regex.Match(html, CARDNAME_FRONT_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!namefront.Groups["name"].Success) throw new Exception("Unable to capture front name; check regex");

			// >> Back Name
			const string CARDNAME_BACK_REGEX = "<div class=\"nameCol\">.*?<.*?class=\"cardName is-back\">(?<name>.*?)</";
			Match nameback = Regex.Match(html, CARDNAME_BACK_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!nameback.Groups["name"].Success) throw new Exception("Unable to capture back name; check regex");

			// >> Type
			const string TYPE_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Card Type|カードタイプ)</.*?>.*?<div class=\"data\">(?<type>.*?)</div>";
			Match type = Regex.Match(html, TYPE_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!type.Groups["type"].Success) throw new Exception("Unable to capture type; check regex");

			// >> Color
			const string COLOR_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Color|色)</.*?>.*?<div class=\"data color.*?\">.*?<div class=\"colValue\" data-color=\"(?<color>.*?)\">";
			Match color = Regex.Match(html, COLOR_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!color.Groups["color"].Success) throw new Exception("Unable to capture color; check regex");

			// >> Front Power
			const string POWER_FRONT_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Power|パワー)</.*?>.*?<div class=\"data is-front\">(?<power>.*?)</div>";
			Match powerfront = Regex.Match(html, POWER_FRONT_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!powerfront.Groups["power"].Success) throw new Exception("Unable to capture front power; check regex");

			// >> Back Power
			const string POWER_BACK_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Power|パワー)</.*?>.*?<div class=\"data is-back\">(?<power>.*?)</div>";
			Match powerback = Regex.Match(html, POWER_BACK_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!powerback.Groups["power"].Success) throw new Exception("Unable to capture back power; check regex");

			// >> Front Features
			const string FEATURES_FRONT_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Features|特徴)</.*?>.*?<div class=\"data is-front\">(?<features>.*?)</div>";
			Match featuresfront = Regex.Match(html, FEATURES_FRONT_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!featuresfront.Groups["features"].Success) throw new Exception("Unable to capture front features; check regex");

			// >> Back Features
			const string FEATURES_BACK_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Features|特徴)</.*?>.*?<div class=\"data is-back\">(?<features>.*?)</div>";
			Match featuresback = Regex.Match(html, FEATURES_BACK_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!featuresback.Groups["features"].Success) throw new Exception("Unable to capture back features; check regex");

			// >> Front Effect
			const string EFFECT_FRONT_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Effect|カード効果).*?<div class=\".*?is-front.*?\">(?<effect>.*?)</div";
			Match effectfront = Regex.Match(html, EFFECT_FRONT_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!effectfront.Groups["effect"].Success) throw new Exception("Unable to capture front effect; check regex");

			// >> Back Effect
			const string EFFECT_BACK_REGEX = "<div class=\"cardDataCell\">.*?<.*?>(Effect|カード効果).*?<div class=\".*?is-back.*?\">(?<effect>.*?)</div";
			Match effectback = Regex.Match(html, EFFECT_BACK_REGEX, RegexOptions.IgnoreCase | RegexOptions.Singleline);
			if(!effectback.Groups["effect"].Success) throw new Exception("Unable to capture back effect; check regex");

			// Power cannot be null for leaders
			if(!int.TryParse(powerfront.Groups["power"].Value.Trim(), out int powerfrontval))
				throw new Exception("Unable to convert front power into an integer");

			ScrapedCardDetail front = new ScrapedCardDetail
			{
				Type = type.Groups["type"].Value.Trim(),
				Color = color.Groups["color"].Value.Trim(),
				Cost = null,
				SpecifiedCost = null,
				Name = namefront.Groups["name"].Value.Trim(),
				Power = powerfrontval,
				ComboPower = null,
				Traits = ParseNullableString(featuresfront.Groups["features"].Value.Trim()),
				Effect = ParseNullableString(WebUtility.HtmlDecode(effectfront.Groups["effect"].Value.Trim()).Replace("<br>", "\n"))
			};

			// Power cannot be null for leaders
			if(!int.TryParse(powerback.Groups["power"].Value.Trim(), out int powerbackval))
				throw new Exception("Unable to convert back power into an integer");

			ScrapedCardDetail back = new ScrapedCardDetail
			{
				Type = type.Groups["type"].Value.Trim(),
				Color = color.Groups["color"].Value.Trim(),
				Cost = null,
				SpecifiedCost = null,
				Name = nameback.Groups["name"].Value.Trim(),
				Power = powerbackval,
				ComboPower = null,
				Traits = ParseNullableString(featuresback.Groups["features"].Value.Trim()),
				Effect = ParseNullableString(WebUtility.HtmlDecode(effectback.Groups["effect"].Value.Trim()).Replace("<br>", "\n"))
			};

			return new Tuple<ScrapedCardDetail, ScrapedCardDetail>(front, back);
		}
	}
}
